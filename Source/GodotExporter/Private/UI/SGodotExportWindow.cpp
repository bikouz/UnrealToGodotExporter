#include "UI/SGodotExportWindow.h"

#include "AssetRegistry/AssetData.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "Framework/Application/SlateApplication.h"
#include "GodotExportPipeline.h"
#include "GodotExportSettings.h"
#include "IContentBrowserSingleton.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "GodotExporter"

void SGodotExportWindow::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	FDetailsViewArgs DetailsArgs;
	DetailsArgs.bAllowSearch = true;
	DetailsArgs.bHideSelectionTip = true;
	DetailsArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	DetailsView = PropertyModule.CreateDetailView(DetailsArgs);
	DetailsView->SetObject(GetMutableDefault<UGodotExportSettings>());

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(8.f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 0.f, 0.f, 8.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Title", "Export Unreal assets to Godot 4"))
			]
			+ SVerticalBox::Slot()
			.FillHeight(0.62f)
			[
				DetailsView.ToSharedRef()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 8.f)
			[
				SNew(SSeparator)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.f, 0.f, 8.f, 0.f)
				[
					SNew(SButton)
					.Text(LOCTEXT("Browse", "Browse Godot Project"))
					.OnClicked(this, &SGodotExportWindow::OnBrowseGodotProject)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.f, 0.f, 8.f, 0.f)
				[
					SNew(SButton)
					.ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("PrimaryButton"))
					.Text(LOCTEXT("ExportAll", "Export All"))
					.OnClicked(this, &SGodotExportWindow::OnExportAll)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.f, 0.f, 8.f, 0.f)
				[
					SNew(SButton)
					.Text(LOCTEXT("ExportSelected", "Export Selected Assets"))
					.OnClicked(this, &SGodotExportWindow::OnExportSelected)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("ExportFolders", "Export Selected Folders"))
					.OnClicked(this, &SGodotExportWindow::OnExportSelectedFolders)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 8.f, 0.f, 4.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("LogLabel", "Last export log"))
			]
			+ SVerticalBox::Slot()
			.FillHeight(0.38f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SAssignNew(LogBox, SMultiLineEditableTextBox)
					.IsReadOnly(true)
					.AutoWrapText(true)
					.Text(FText::FromString(TEXT("Ready.")))
				]
			]
		]
	];
}

FReply SGodotExportWindow::OnBrowseGodotProject()
{
	FString Path = GetDefault<UGodotExportSettings>()->GodotProjectPath.Path;
	if (FGodotExportPipeline::PromptForGodotProject(Path))
	{
		if (UGodotExportSettings* Settings = GetMutableDefault<UGodotExportSettings>())
		{
			Settings->GodotProjectPath.Path = Path;
			Settings->SaveConfig();
			if (DetailsView.IsValid())
			{
				DetailsView->ForceRefresh();
			}
		}
	}
	return FReply::Handled();
}

FReply SGodotExportWindow::OnExportAll()
{
	if (UGodotExportSettings* Settings = GetMutableDefault<UGodotExportSettings>())
	{
		Settings->SaveConfig();
	}
	ApplyResult(FGodotExportPipeline::ExportAll());
	return FReply::Handled();
}

FReply SGodotExportWindow::OnExportSelected()
{
	if (UGodotExportSettings* Settings = GetMutableDefault<UGodotExportSettings>())
	{
		Settings->SaveConfig();
	}

	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FAssetData> Selected;
	ContentBrowser.Get().GetSelectedAssets(Selected);
	ApplyResult(FGodotExportPipeline::ExportAssets(Selected));
	return FReply::Handled();
}

FReply SGodotExportWindow::OnExportSelectedFolders()
{
	if (UGodotExportSettings* Settings = GetMutableDefault<UGodotExportSettings>())
	{
		Settings->SaveConfig();
	}

	FContentBrowserModule& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FString> Folders;
	ContentBrowser.Get().GetSelectedFolders(Folders);

	if (Folders.Num() == 0)
	{
		LogText = TEXT("No folder selected in the Content Browser.");
		if (LogBox.IsValid())
		{
			LogBox->SetText(FText::FromString(LogText));
		}
		return FReply::Handled();
	}

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
	ApplyResult(Combined);
	return FReply::Handled();
}

void SGodotExportWindow::ApplyResult(const FGodotExportResult& Result)
{
	TStringBuilder<2048> Builder;
	Builder.Append(Result.ToSummary());
	Builder.Append(TEXT("\n\n"));

	const int32 MaxLines = 200;
	const int32 Count = FMath::Min(Result.Items.Num(), MaxLines);
	for (int32 i = 0; i < Count; ++i)
	{
		const FGodotExportItemResult& Item = Result.Items[i];
		const UEnum* Enum = StaticEnum<EGodotExportStatus>();
		const FString Status = Enum ? Enum->GetNameStringByValue(static_cast<int64>(Item.Status)) : TEXT("?");
		Builder.Appendf(TEXT("[%s] %s -> %s (%s)\n"), *Status, *Item.AssetPath, *Item.OutputPath, *Item.Message);
	}
	if (Result.Items.Num() > MaxLines)
	{
		Builder.Appendf(TEXT("... %d more items in godot_export_manifest.json\n"), Result.Items.Num() - MaxLines);
	}

	LogText = Builder.ToString();
	if (LogBox.IsValid())
	{
		LogBox->SetText(FText::FromString(LogText));
	}
}

#undef LOCTEXT_NAMESPACE
