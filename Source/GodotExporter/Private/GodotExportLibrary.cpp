#include "GodotExportLibrary.h"

#include "AssetRegistry/AssetData.h"
#include "GodotExportPipeline.h"
#include "GodotExportSettings.h"

namespace
{
	UGodotExportSettings* MakeTransientSettings(const FString& GodotProjectPath)
	{
		UGodotExportSettings* Settings = DuplicateObject<UGodotExportSettings>(GetDefault<UGodotExportSettings>(), GetTransientPackage());
		if (!GodotProjectPath.IsEmpty())
		{
			Settings->GodotProjectPath.Path = GodotProjectPath;
		}
		return Settings;
	}

	TArray<FAssetData> ToAssetData(const TArray<UObject*>& Assets)
	{
		TArray<FAssetData> Result;
		for (UObject* Asset : Assets)
		{
			if (Asset)
			{
				Result.Emplace(Asset);
			}
		}
		return Result;
	}
}

FGodotExportResult UGodotExportLibrary::ExportAssetToGodot(UObject* Asset, const FString& GodotProjectPath)
{
	if (!Asset)
	{
		return FGodotExportResult();
	}

	TArray<UObject*> Assets;
	Assets.Add(Asset);
	return ExportAssetsToGodot(Assets, GodotProjectPath);
}

FGodotExportResult UGodotExportLibrary::ExportAssetsToGodot(const TArray<UObject*>& Assets, const FString& GodotProjectPath)
{
	UGodotExportSettings* Settings = MakeTransientSettings(GodotProjectPath);
	return FGodotExportPipeline::ExportAssets(ToAssetData(Assets), Settings, GodotProjectPath.IsEmpty());
}

FGodotExportResult UGodotExportLibrary::ExportContentFolderToGodot(const FString& UnrealContentPath, const FString& GodotProjectPath)
{
	UGodotExportSettings* Settings = MakeTransientSettings(GodotProjectPath);
	return FGodotExportPipeline::ExportContentPath(UnrealContentPath, Settings, GodotProjectPath.IsEmpty());
}

FGodotExportResult UGodotExportLibrary::ExportAllToGodot()
{
	return FGodotExportPipeline::ExportAll(GetDefault<UGodotExportSettings>());
}
