// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GodotExportSettings.h"
#include "UObject/SoftObjectPath.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGodotExportSettings() {}

// ********** Begin Cross Module References ********************************************************
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FDirectoryPath(ETypeConstructPhase);
DEVELOPERSETTINGS_API UClass* Z_Construct_UClass_UDeveloperSettings(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase);
GODOTEXPORTER_API UClass* Z_Construct_UClass_UGodotExportSettings(ETypeConstructPhase);
GODOTEXPORTER_API UClass* Z_Construct_UClass_UGodotExportSettings(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UGodotExportSettings *****************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UGodotExportSettings_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "DisplayName", "Godot Exporter" },
		{ "IncludePath", "GodotExportSettings.h" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GodotProjectPath_MetaData[] = {
		{ "Category", "Output" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Absolute path to the Godot 4 project folder (the folder that contains or will contain project.godot). */" },
#endif
		{ "DisplayName", "Godot Project Folder" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Absolute path to the Godot 4 project folder (the folder that contains or will contain project.godot)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ExportSubfolder_MetaData[] = {
		{ "Category", "Output" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Subfolder created at the destination (assets/export/meshes, materials, textures, anims, ...). */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Subfolder created at the destination (assets/export/meshes, materials, textures, anims, ...)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_SourceContentPath_MetaData[] = {
		{ "Category", "Output" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Unreal content root to export when using Export All. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Unreal content root to export when using Export All." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bUseBinaryGlb_MetaData[] = {
		{ "Category", "Output" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Export meshes as binary .glb (recommended). If false, export .gltf + sidecar files. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Export meshes as binary .glb (recommended). If false, export .gltf + sidecar files." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bWriteProjectGodotIfMissing_MetaData[] = {
		{ "Category", "Output" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Write a project.godot file if the destination folder is not already a Godot project. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Write a project.godot file if the destination folder is not already a Godot project." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bWriteManifest_MetaData[] = {
		{ "Category", "Output" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Write godot_export_manifest.json after each batch. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Write godot_export_manifest.json after each batch." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportStaticMeshes_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportSkeletalMeshes_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportAnimations_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportTextures_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportMaterials_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportAudio_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportLevels_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportDataTables_MetaData[] = {
		{ "Category", "Asset Types" },
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIncludeDependencies_MetaData[] = {
		{ "Category", "Pipeline" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** When exporting a mesh, material or level selection, also export referenced textures/materials. AnimSequences never pull dependencies. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "When exporting a mesh, material or level selection, also export referenced textures/materials. AnimSequences never pull dependencies." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bSkipUnchanged_MetaData[] = {
		{ "Category", "Pipeline" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Skip an asset when the destination file is newer than the Unreal package. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Skip an asset when the destination file is newer than the Unreal package." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bSkipDeveloperFolders_MetaData[] = {
		{ "Category", "Pipeline" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Ignore /Game/Developers while exporting. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Ignore /Game/Developers while exporting." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bAutoExportOnSave_MetaData[] = {
		{ "Category", "Pipeline" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Automatically export a saved asset if it is a supported type. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Automatically export a saved asset if it is a supported type." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bReplaceSpacesInPaths_MetaData[] = {
		{ "Category", "Pipeline" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Replace spaces in exported relative paths. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Replace spaces in exported relative paths." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bPreferOriginalSourceFiles_MetaData[] = {
		{ "Category", "Pipeline" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Copy original imported PNG/WAV/OGG when the source file still exists on disk. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Copy original imported PNG/WAV/OGG when the source file still exists on disk." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_UniformScale_MetaData[] = {
		{ "Category", "Conversion" },
		{ "ClampMax", "100.0" },
		{ "ClampMin", "0.0001" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Unreal uses centimeters. Godot and glTF use meters. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Unreal uses centimeters. Godot and glTF use meters." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bFlipNormalMapGreenChannel_MetaData[] = {
		{ "Category", "Conversion" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Flip the green channel of normal maps (DirectX -> OpenGL / Godot). */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Flip the green channel of normal maps (DirectX -> OpenGL / Godot)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bGenerateGodotMaterials_MetaData[] = {
		{ "Category", "Conversion" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Generate Godot StandardMaterial3D .tres files from Unreal materials. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Generate Godot StandardMaterial3D .tres files from Unreal materials." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bExportPrefabs_MetaData[] = {
		{ "Category", "Conversion" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Write a Godot PackedScene .tscn in prefabs/ for each static or skeletal mesh. */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Write a Godot PackedScene .tscn in prefabs/ for each static or skeletal mesh." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bGenerateStaticMeshPhysics_MetaData[] = {
		{ "Category", "Conversion" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Static mesh .glb.import: generate StaticBody3D + single convex collision (Godot physics/shape_type = 1). */" },
#endif
		{ "ModuleRelativePath", "Public/GodotExportSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Static mesh .glb.import: generate StaticBody3D + single convex collision (Godot physics/shape_type = 1)." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UGodotExportSettings constinit property declarations *********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_GodotProjectPath;
	static const UECodeGen_Private::FStrPropertyParams NewProp_ExportSubfolder;
	static const UECodeGen_Private::FStrPropertyParams NewProp_SourceContentPath;
	static void NewProp_bUseBinaryGlb_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bUseBinaryGlb = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bUseBinaryGlb;
	static void NewProp_bWriteProjectGodotIfMissing_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bWriteProjectGodotIfMissing = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bWriteProjectGodotIfMissing;
	static void NewProp_bWriteManifest_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bWriteManifest = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bWriteManifest;
	static void NewProp_bExportStaticMeshes_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportStaticMeshes = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportStaticMeshes;
	static void NewProp_bExportSkeletalMeshes_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportSkeletalMeshes = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportSkeletalMeshes;
	static void NewProp_bExportAnimations_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportAnimations = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportAnimations;
	static void NewProp_bExportTextures_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportTextures = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportTextures;
	static void NewProp_bExportMaterials_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportMaterials = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportMaterials;
	static void NewProp_bExportAudio_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportAudio = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportAudio;
	static void NewProp_bExportLevels_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportLevels = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportLevels;
	static void NewProp_bExportDataTables_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportDataTables = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportDataTables;
	static void NewProp_bIncludeDependencies_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bIncludeDependencies = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIncludeDependencies;
	static void NewProp_bSkipUnchanged_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bSkipUnchanged = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bSkipUnchanged;
	static void NewProp_bSkipDeveloperFolders_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bSkipDeveloperFolders = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bSkipDeveloperFolders;
	static void NewProp_bAutoExportOnSave_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bAutoExportOnSave = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bAutoExportOnSave;
	static void NewProp_bReplaceSpacesInPaths_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bReplaceSpacesInPaths = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bReplaceSpacesInPaths;
	static void NewProp_bPreferOriginalSourceFiles_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bPreferOriginalSourceFiles = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bPreferOriginalSourceFiles;
	static const UECodeGen_Private::FFloatPropertyParams NewProp_UniformScale;
	static void NewProp_bFlipNormalMapGreenChannel_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bFlipNormalMapGreenChannel = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bFlipNormalMapGreenChannel;
	static void NewProp_bGenerateGodotMaterials_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bGenerateGodotMaterials = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bGenerateGodotMaterials;
	static void NewProp_bExportPrefabs_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bExportPrefabs = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bExportPrefabs;
	static void NewProp_bGenerateStaticMeshPhysics_SetBit(void* Obj)
	{
		((UGodotExportSettings*)Obj)->bGenerateStaticMeshPhysics = 1;
	}
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bGenerateStaticMeshPhysics;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Class UGodotExportSettings constinit property declarations ***********************
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGodotExportSettings>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS

// ********** Begin Class UGodotExportSettings Property Definitions ********************************
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_GodotProjectPath = { "GodotProjectPath", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(UGodotExportSettings, GodotProjectPath), Z_Construct_UScriptStruct_FDirectoryPath, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GodotProjectPath_MetaData), NewProp_GodotProjectPath_MetaData) }; // a9ea974572ee54d1b93cb49a6434fdbad40b6bf5
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_ExportSubfolder = { "ExportSubfolder", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGodotExportSettings, ExportSubfolder), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ExportSubfolder_MetaData), NewProp_ExportSubfolder_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_SourceContentPath = { "SourceContentPath", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(UGodotExportSettings, SourceContentPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_SourceContentPath_MetaData), NewProp_SourceContentPath_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bUseBinaryGlb = { "bUseBinaryGlb", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bUseBinaryGlb_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bUseBinaryGlb_MetaData), NewProp_bUseBinaryGlb_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bWriteProjectGodotIfMissing = { "bWriteProjectGodotIfMissing", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bWriteProjectGodotIfMissing_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bWriteProjectGodotIfMissing_MetaData), NewProp_bWriteProjectGodotIfMissing_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bWriteManifest = { "bWriteManifest", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bWriteManifest_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bWriteManifest_MetaData), NewProp_bWriteManifest_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportStaticMeshes = { "bExportStaticMeshes", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportStaticMeshes_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportStaticMeshes_MetaData), NewProp_bExportStaticMeshes_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportSkeletalMeshes = { "bExportSkeletalMeshes", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportSkeletalMeshes_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportSkeletalMeshes_MetaData), NewProp_bExportSkeletalMeshes_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportAnimations = { "bExportAnimations", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportAnimations_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportAnimations_MetaData), NewProp_bExportAnimations_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportTextures = { "bExportTextures", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportTextures_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportTextures_MetaData), NewProp_bExportTextures_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportMaterials = { "bExportMaterials", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportMaterials_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportMaterials_MetaData), NewProp_bExportMaterials_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportAudio = { "bExportAudio", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportAudio_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportAudio_MetaData), NewProp_bExportAudio_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportLevels = { "bExportLevels", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportLevels_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportLevels_MetaData), NewProp_bExportLevels_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportDataTables = { "bExportDataTables", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportDataTables_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportDataTables_MetaData), NewProp_bExportDataTables_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bIncludeDependencies = { "bIncludeDependencies", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bIncludeDependencies_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIncludeDependencies_MetaData), NewProp_bIncludeDependencies_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bSkipUnchanged = { "bSkipUnchanged", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bSkipUnchanged_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bSkipUnchanged_MetaData), NewProp_bSkipUnchanged_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bSkipDeveloperFolders = { "bSkipDeveloperFolders", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bSkipDeveloperFolders_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bSkipDeveloperFolders_MetaData), NewProp_bSkipDeveloperFolders_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bAutoExportOnSave = { "bAutoExportOnSave", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bAutoExportOnSave_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bAutoExportOnSave_MetaData), NewProp_bAutoExportOnSave_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bReplaceSpacesInPaths = { "bReplaceSpacesInPaths", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bReplaceSpacesInPaths_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bReplaceSpacesInPaths_MetaData), NewProp_bReplaceSpacesInPaths_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bPreferOriginalSourceFiles = { "bPreferOriginalSourceFiles", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bPreferOriginalSourceFiles_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bPreferOriginalSourceFiles_MetaData), NewProp_bPreferOriginalSourceFiles_MetaData) };
const UECodeGen_Private::FFloatPropertyParams UHT_STATICS::NewProp_UniformScale = { "UniformScale", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, nullptr, nullptr, 1, STRUCT_OFFSET(UGodotExportSettings, UniformScale), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_UniformScale_MetaData), NewProp_UniformScale_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bFlipNormalMapGreenChannel = { "bFlipNormalMapGreenChannel", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bFlipNormalMapGreenChannel_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bFlipNormalMapGreenChannel_MetaData), NewProp_bFlipNormalMapGreenChannel_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bGenerateGodotMaterials = { "bGenerateGodotMaterials", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bGenerateGodotMaterials_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bGenerateGodotMaterials_MetaData), NewProp_bGenerateGodotMaterials_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bExportPrefabs = { "bExportPrefabs", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bExportPrefabs_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bExportPrefabs_MetaData), NewProp_bExportPrefabs_MetaData) };
const UECodeGen_Private::FBoolPropertyParams UHT_STATICS::NewProp_bGenerateStaticMeshPhysics = { "bGenerateStaticMeshPhysics", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, nullptr, nullptr, 1, sizeof(bool), sizeof(UGodotExportSettings), &UHT_STATICS::NewProp_bGenerateStaticMeshPhysics_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bGenerateStaticMeshPhysics_MetaData), NewProp_bGenerateStaticMeshPhysics_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GodotProjectPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ExportSubfolder,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_SourceContentPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bUseBinaryGlb,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bWriteProjectGodotIfMissing,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bWriteManifest,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportStaticMeshes,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportSkeletalMeshes,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportAnimations,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportTextures,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportMaterials,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportAudio,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportLevels,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportDataTables,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bIncludeDependencies,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bSkipUnchanged,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bSkipDeveloperFolders,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bAutoExportOnSave,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bReplaceSpacesInPaths,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bPreferOriginalSourceFiles,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_UniformScale,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bFlipNormalMapGreenChannel,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bGenerateGodotMaterials,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bExportPrefabs,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_bGenerateStaticMeshPhysics,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Class UGodotExportSettings Property Definitions **********************************
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_UDeveloperSettings,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GodotExporter,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UGodotExportSettings,
	"Editor",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	UHT_STATICS::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	0,
	0x001000A6u,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
FClassRegistrationInfo Z_Registration_Info_UClass_UGodotExportSettings;
UClass* Z_Construct_UClass_UGodotExportSettings(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UGodotExportSettings;
		if (!Z_Registration_Info_UClass_UGodotExportSettings.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("GodotExportSettings"),
				Z_Registration_Info_UClass_UGodotExportSettings.InnerSingleton,
				nullptr,
				DataSizeOf<TClass>(),
				alignof(TClass),
				TClass::StaticClassFlags,
				TClass::StaticClassCastFlags(),
				TClass::StaticConfigName(),
				(UClass::ClassConstructorType)InternalConstructor<TClass>,
				(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
				UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
				&TClass::Super::StaticClass,
				&TClass::WithinClass::StaticClass
			);
		}
		return Z_Registration_Info_UClass_UGodotExportSettings.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UGodotExportSettings.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGodotExportSettings.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UGodotExportSettings.OuterSingleton;
}
#undef UHT_STATICS
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGodotExportSettings);
UGodotExportSettings::~UGodotExportSettings() {}
// ********** End Class UGodotExportSettings *******************************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportSettings_h__Script_GodotExporter_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGodotExportSettings, TEXT("UGodotExportSettings"), &Z_Registration_Info_UClass_UGodotExportSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGodotExportSettings), 1829903816U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportSettings_h__Script_GodotExporter_ba31f6b3104a3303a545bfb24ed6550e466804fc{
	TEXT("/Script/GodotExporter"),
	UHT_STATICS::ClassInfo, UE_ARRAY_COUNT(UHT_STATICS::ClassInfo),
	nullptr, 0,
	nullptr, 0,
	nullptr, 0,
};
#undef UHT_STATICS
// ********** End Registration *********************************************************************
#undef UHT_STRUCT_BASE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
