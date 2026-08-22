#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/EngineTypes.h"
#include "GodotExportSettings.generated.h"

UCLASS(Config = Editor, DefaultConfig, meta = (DisplayName = "Godot Exporter"))
class GODOTEXPORTER_API UGodotExportSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGodotExportSettings();

	virtual FName GetCategoryName() const override { return FName(TEXT("Plugins")); }
	virtual FName GetSectionName() const override { return FName(TEXT("GodotExporter")); }

	/** Absolute path to the Godot 4 project folder (the folder that contains or will contain project.godot). */
	UPROPERTY(Config, EditAnywhere, Category = "Output", meta = (DisplayName = "Godot Project Folder"))
	FDirectoryPath GodotProjectPath;

	/** Subfolder created at the destination (assets/export/meshes, materials, textures, anims, ...). */
	UPROPERTY(Config, EditAnywhere, Category = "Output")
	FString ExportSubfolder = TEXT("assets/export");

	/** Unreal content root to export when using Export All. */
	UPROPERTY(Config, EditAnywhere, Category = "Output")
	FString SourceContentPath = TEXT("/Game");

	/** Export meshes as binary .glb (recommended). If false, export .gltf + sidecar files. */
	UPROPERTY(Config, EditAnywhere, Category = "Output")
	bool bUseBinaryGlb = true;

	/** Write a project.godot file if the destination folder is not already a Godot project. */
	UPROPERTY(Config, EditAnywhere, Category = "Output")
	bool bWriteProjectGodotIfMissing = true;

	/** Write godot_export_manifest.json after each batch. */
	UPROPERTY(Config, EditAnywhere, Category = "Output")
	bool bWriteManifest = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportStaticMeshes = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportSkeletalMeshes = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportAnimations = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportTextures = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportMaterials = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportAudio = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportLevels = true;

	UPROPERTY(Config, EditAnywhere, Category = "Asset Types")
	bool bExportDataTables = true;

	/** When exporting a mesh, material or level selection, also export referenced textures/materials. AnimSequences never pull dependencies. */
	UPROPERTY(Config, EditAnywhere, Category = "Pipeline")
	bool bIncludeDependencies = true;

	/** Skip an asset when the destination file is newer than the Unreal package. */
	UPROPERTY(Config, EditAnywhere, Category = "Pipeline")
	bool bSkipUnchanged = true;

	/** Ignore /Game/Developers while exporting. */
	UPROPERTY(Config, EditAnywhere, Category = "Pipeline")
	bool bSkipDeveloperFolders = true;

	/** Automatically export a saved asset if it is a supported type. */
	UPROPERTY(Config, EditAnywhere, Category = "Pipeline")
	bool bAutoExportOnSave = false;

	/** Replace spaces in exported relative paths. */
	UPROPERTY(Config, EditAnywhere, Category = "Pipeline")
	bool bReplaceSpacesInPaths = true;

	/** Copy original imported PNG/WAV/OGG when the source file still exists on disk. */
	UPROPERTY(Config, EditAnywhere, Category = "Pipeline")
	bool bPreferOriginalSourceFiles = true;

	/** Unreal uses centimeters. Godot and glTF use meters. */
	UPROPERTY(Config, EditAnywhere, Category = "Conversion", meta = (ClampMin = "0.0001", ClampMax = "100.0"))
	float UniformScale = 0.01f;

	/** Flip the green channel of normal maps (DirectX -> OpenGL / Godot). */
	UPROPERTY(Config, EditAnywhere, Category = "Conversion")
	bool bFlipNormalMapGreenChannel = true;

	/** Generate Godot StandardMaterial3D .tres files from Unreal materials. */
	UPROPERTY(Config, EditAnywhere, Category = "Conversion")
	bool bGenerateGodotMaterials = true;
};
