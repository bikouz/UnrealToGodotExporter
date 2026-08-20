#pragma once

#include "CoreMinimal.h"
#include "GodotExportTypes.h"

class UGodotExportSettings;
struct FAssetData;

class GODOTEXPORTER_API FGodotExportPipeline
{
public:
	/** Tools > Godot Exporter: write into a Godot project (project.godot, folders, manifest). */
	static FGodotExportResult ExportAssets(
		const TArray<FAssetData>& InAssets,
		const UGodotExportSettings* Settings = nullptr,
		bool bPromptForProjectIfMissing = true);

	static FGodotExportResult ExportContentPath(
		const FString& UnrealContentPath,
		const UGodotExportSettings* Settings = nullptr,
		bool bPromptForProjectIfMissing = true);

	static FGodotExportResult ExportAll(const UGodotExportSettings* Settings = nullptr);

	/** Content Browser right-click: export files only, never create a Godot project. */
	static FGodotExportResult ExportAssetsStandalone(
		const TArray<FAssetData>& InAssets,
		const UGodotExportSettings* Settings = nullptr);

	static FGodotExportResult ExportContentPathStandalone(
		const FString& UnrealContentPath,
		const UGodotExportSettings* Settings = nullptr);

	static bool IsSingleAnimSequenceSelection(const TArray<FAssetData>& Assets, FAssetData& OutAnim);
	static bool IsSupportedClass(const UClass* Class, const UGodotExportSettings* Settings = nullptr);
	static bool PromptForGodotProject(FString& OutPath);
	static bool PromptForDirectory(const FString& Title, FString& OutPath);
	static FString ResolveGodotProjectPath(const UGodotExportSettings* Settings, bool bPromptIfMissing);

	static FString PackageToResPath(const FString& PackageName, const FString& Extension, const UGodotExportSettings* Settings, const FAssetData& Asset = FAssetData());
	static FString PackageToAbsolutePath(const FString& GodotProject, const FString& PackageName, const FString& Extension, const UGodotExportSettings* Settings, const FAssetData& Asset = FAssetData());

private:
	explicit FGodotExportPipeline(const UGodotExportSettings* InSettings, const FString& InOutputRoot, bool bInGodotProjectMode = true);

	FGodotExportResult Run(TArray<FAssetData> Assets, bool bExpandDependencies);
	FGodotExportResult ExportStandaloneSingle(const FAssetData& AssetData);
	FString ResolveOutputPath(const FAssetData& AssetData, const FString& Extension) const;
	static FString DefaultExtensionForAsset(const FAssetData& AssetData, const UGodotExportSettings* Settings);
	static FString SaveDialogFilterForAsset(const FAssetData& AssetData, const UGodotExportSettings* Settings);
	void ExpandDependencies(TArray<FAssetData>& Assets) const;
	void SortForExport(TArray<FAssetData>& Assets) const;
	FGodotExportItemResult ExportOne(const FAssetData& AssetData);

	bool ExportMeshOrAnim(UObject* Object, const FAssetData& AssetData, FGodotExportItemResult& Item);
	void CollectMeshMaterials(UObject* MeshObject, TArray<class UMaterialInterface*>& OutMaterials) const;
	void ExportMeshSidecars(UObject* MeshObject);
	bool ExportTexture(class UTexture2D* Texture, const FAssetData& AssetData, FGodotExportItemResult& Item);
	bool ExportAudio(class USoundWave* SoundWave, const FAssetData& AssetData, FGodotExportItemResult& Item);
	bool ExportMaterial(class UMaterialInterface* Material, const FAssetData& AssetData, FGodotExportItemResult& Item);
	bool ExportLevel(class UWorld* World, const FAssetData& AssetData, FGodotExportItemResult& Item);
	bool ExportDataTable(class UDataTable* DataTable, const FAssetData& AssetData, FGodotExportItemResult& Item);

	bool ExportWithGltf(UObject* Object, const FString& AbsolutePath, FString& OutMessage);
	void CleanupGltfImagesInMeshFolder(const FString& GltfAbsolutePath);
	bool WriteProjectGodotIfNeeded() const;
	void WriteManifest(const FGodotExportResult& Result) const;
	bool ShouldSkipUnchanged(const FAssetData& AssetData, const FString& DestinationFile) const;
	bool CopyOriginalSourceFile(UObject* Asset, const FString& DestinationFile, FString& OutUsedExtension) const;

	const UGodotExportSettings* Settings = nullptr;
	FString GodotProject;
	FString OutputPathOverride;
	bool bGodotProjectMode = true;
	bool bFlattenSidecars = false;
	TMap<FName, FString> ExportedResPaths;
};
