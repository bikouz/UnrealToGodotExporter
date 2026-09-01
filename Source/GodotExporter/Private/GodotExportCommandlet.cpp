#include "GodotExportCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "GodotExportPipeline.h"
#include "GodotExportSettings.h"
#include "Misc/Parse.h"

UGodotExportCommandlet::UGodotExportCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;

	HelpDescription = TEXT("Export Unreal assets to a Godot 4 project.");
	HelpUsage = TEXT("UnrealEditor-Cmd.exe <project>.uproject -run=GodotExport -GodotProject=<dir> [-Path=/Game]");
	HelpParamNames.Add(TEXT("GodotProject"));
	HelpParamDescriptions.Add(TEXT("Absolute path to the Godot project folder."));
	HelpParamNames.Add(TEXT("Path"));
	HelpParamDescriptions.Add(TEXT("Unreal content path to export. Defaults to /Game."));
}

int32 UGodotExportCommandlet::Main(const FString& Params)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().SearchAllAssets(true);

	UGodotExportSettings* Settings = DuplicateObject<UGodotExportSettings>(GetDefault<UGodotExportSettings>(), GetTransientPackage());

	FString GodotProject;
	FString ContentPath;
	FParse::Value(*Params, TEXT("GodotProject="), GodotProject);
	FParse::Value(*Params, TEXT("Path="), ContentPath);

	if (!GodotProject.IsEmpty())
	{
		Settings->GodotProjectPath.Path = GodotProject;
	}
	if (!ContentPath.IsEmpty())
	{
		Settings->SourceContentPath = ContentPath;
	}

	if (Settings->GodotProjectPath.Path.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("GodotExport: missing -GodotProject= path."));
		return 1;
	}

	const FGodotExportResult Result = FGodotExportPipeline::ExportContentPath(Settings->SourceContentPath, Settings, false);
	UE_LOG(LogTemp, Display, TEXT("%s"), *Result.ToSummary());
	return Result.NumFailed > 0 ? 2 : 0;
}
