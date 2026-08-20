#include "GodotExporterModule.h"

#include "AssetRegistry/AssetData.h"
#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GodotExportPipeline.h"
#include "GodotExportSettings.h"
#include "IContentBrowserSingleton.h"
#include "LevelEditor.h"
#include "Misc/PackageName.h"
#include "Styling/AppStyle.h"
#include "ToolMenus.h"
#include "UI/SGodotExportWindow.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "GodotExporter"

static const FName GodotExporterTabName(TEXT("GodotExporterTab"));

void FGodotExporterModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FGodotExporterModule::RegisterMenus));

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		GodotExporterTabName,
		FOnSpawnTab::CreateRaw(this, &FGodotExporterModule::SpawnExportTab))
		.SetDisplayName(LOCTEXT("TabTitle", "Godot Exporter"))
		.SetTooltipText(LOCTEXT("TabTooltip", "Export Unreal assets to a Godot 4 project"))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Save"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory());

	PackageSavedHandle = UPackage::PackageSavedWithContextEvent.AddRaw(
		this,
		&FGodotExporterModule::OnPackageSaved);
}

void FGodotExporterModule::ShutdownModule()
{
	if (PackageSavedHandle.IsValid())
	{
		UPackage::PackageSavedWithContextEvent.Remove(PackageSavedHandle);
		PackageSavedHandle.Reset();
	}

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(GodotExporterTabName);
}

void FGodotExporterModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools"))
	{
		FToolMenuSection& Section = Menu->AddSection("GodotExporter", LOCTEXT("Section", "Godot"));
		Section.AddMenuEntry(
			"OpenGodotExporter",
			LOCTEXT("OpenWindow", "Godot Exporter"),
			LOCTEXT("OpenWindowTooltip", "Open the Godot export window (Godot project, batch export)"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Save"),
			FUIAction(FExecuteAction::CreateRaw(this, &FGodotExporterModule::OpenExportWindow)));
	}

	if (UToolMenu* ExistingAssetMenu = UToolMenus::Get()->FindMenu("ContentBrowser.AssetContextMenu"))
	{
		ExistingAssetMenu->RemoveSection("GodotExporter");
	}

	if (UToolMenu* AssetMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu"))
	{
		FToolMenuSection& Section = AssetMenu->AddSection("GodotExporter", LOCTEXT("AssetSection", "Godot"));
		Section.AddDynamicEntry("ExportToGodot", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
		{
			const UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
			if (!Context || Context->SelectedAssets.Num() == 0)
			{
				return;
			}

			const TArray<FAssetData> Assets = Context->SelectedAssets;
			InSection.AddMenuEntry(
				"GodotExporter_ExportToGodot",
				LOCTEXT("ContextExport", "Export to Godot"),
				LOCTEXT("ContextExportTooltip", "Export the selected assets to Godot"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Save"),
				FUIAction(FExecuteAction::CreateLambda([Assets]()
				{
					const FGodotExportResult Result = FGodotExportPipeline::ExportAssetsStandalone(Assets);
					FNotificationInfo Info(FText::FromString(Result.ToSummary()));
					Info.ExpireDuration = 5.0f;
					FSlateNotificationManager::Get().AddNotification(Info);
				})));
		}));
	}

	if (UToolMenu* FolderMenu = UToolMenus::Get()->ExtendMenu("ContentBrowser.FolderContextMenu"))
	{
		FToolMenuSection& Section = FolderMenu->AddSection("GodotExporter", LOCTEXT("FolderSection", "Godot"));
		Section.AddDynamicEntry("ExportFolderToGodot", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
		{
			const UContentBrowserFolderContext* Context = InSection.FindContext<UContentBrowserFolderContext>();
			TArray<FString> Folders = Context ? Context->SelectedPackagePaths : TArray<FString>();
			if (Folders.Num() == 0)
			{
				return;
			}

			InSection.AddMenuEntry(
				"ExportFolderToGodot",
				LOCTEXT("ContextExportFolder", "Export Folder to Godot"),
				LOCTEXT("ContextExportFolderTooltip", "Export this folder as files (no Godot project is created)"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Save"),
				FUIAction(FExecuteAction::CreateLambda([Folders]()
				{
					FGodotExportResult Combined;
					for (const FString& Folder : Folders)
					{
						const FGodotExportResult Part = FGodotExportPipeline::ExportContentPathStandalone(Folder);
						Combined.NumSucceeded += Part.NumSucceeded;
						Combined.NumFailed += Part.NumFailed;
						Combined.NumSkipped += Part.NumSkipped;
						Combined.NumUnsupported += Part.NumUnsupported;
						Combined.Items.Append(Part.Items);
					}

					FNotificationInfo Info(FText::FromString(Combined.ToSummary()));
					Info.ExpireDuration = 5.0f;
					FSlateNotificationManager::Get().AddNotification(Info);
				})));
		}));
	}
}

void FGodotExporterModule::UnregisterMenus()
{
	UToolMenus::UnregisterOwner(this);
}

void FGodotExporterModule::OpenExportWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(GodotExporterTabName);
}

TSharedRef<SDockTab> FGodotExporterModule::SpawnExportTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SGodotExportWindow)
		];
}

void FGodotExporterModule::ExportSelectedAssets()
{
	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FAssetData> Selected;
	ContentBrowser.Get().GetSelectedAssets(Selected);

	const FGodotExportResult Result = FGodotExportPipeline::ExportAssets(Selected);
	FNotificationInfo Info(FText::FromString(Result.ToSummary()));
	Info.ExpireDuration = 5.0f;
	FSlateNotificationManager::Get().AddNotification(Info);
}

void FGodotExporterModule::ExportSelectedFolders()
{
	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FString> Folders;
	ContentBrowser.Get().GetSelectedFolders(Folders);

	FGodotExportResult Combined;
	for (const FString& Folder : Folders)
	{
		const FGodotExportResult Part = FGodotExportPipeline::ExportContentPath(Folder);
		Combined.NumSucceeded += Part.NumSucceeded;
		Combined.NumFailed += Part.NumFailed;
		Combined.NumSkipped += Part.NumSkipped;
		Combined.NumUnsupported += Part.NumUnsupported;
		Combined.Items.Append(Part.Items);
	}

	FNotificationInfo Info(FText::FromString(Combined.ToSummary()));
	Info.ExpireDuration = 5.0f;
	FSlateNotificationManager::Get().AddNotification(Info);
}

void FGodotExporterModule::OnPackageSaved(const FString& PackageFilename, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	if (bIsAutoExporting || !Package)
	{
		return;
	}

	const UGodotExportSettings* Settings = GetDefault<UGodotExportSettings>();
	if (!Settings || !Settings->bAutoExportOnSave || Settings->GodotProjectPath.Path.IsEmpty())
	{
		return;
	}

	if (ObjectSaveContext.IsProceduralSave() || ObjectSaveContext.IsFromAutoSave())
	{
		return;
	}

	FString PackageName;
	if (!FPackageName::TryConvertFilenameToLongPackageName(PackageFilename, PackageName))
	{
		PackageName = Package->GetName();
	}

	if (!PackageName.StartsWith(TEXT("/Game")))
	{
		return;
	}

	TArray<UObject*> Objects;
	GetObjectsWithPackage(Package, Objects);

	TArray<FAssetData> Assets;
	for (UObject* Object : Objects)
	{
		if (Object && Object->IsAsset() && FGodotExportPipeline::IsSupportedClass(Object->GetClass(), Settings))
		{
			Assets.Emplace(Object);
		}
	}

	if (Assets.Num() == 0)
	{
		return;
	}

	bIsAutoExporting = true;
	FGodotExportPipeline::ExportAssets(Assets, Settings, false);
	bIsAutoExporting = false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGodotExporterModule, GodotExporter)
