// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GodotExportLibrary.h"
#include "GodotExportTypes.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGodotExportLibrary() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary(ETypeConstructPhase);
COREUOBJECT_API UClass* Z_Construct_UClass_UObject(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase);
GODOTEXPORTER_API UClass* Z_Construct_UClass_UGodotExportLibrary(ETypeConstructPhase);
GODOTEXPORTER_API UScriptStruct* Z_Construct_UScriptStruct_FGodotExportResult(ETypeConstructPhase);
GODOTEXPORTER_API UClass* Z_Construct_UClass_UGodotExportLibrary(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UGodotExportLibrary Function ExportAllToGodot ****************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGodotExportLibrary_ExportAllToGodot_Statics
struct UHT_STATICS
{
	struct GodotExportLibrary_eventExportAllToGodot_Parms
	{
		FGodotExportResult ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Godot Export" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Export the whole Source Content Path from plugin settings. */" },
#endif
		{ "DevelopmentOnly", "" },
		{ "ModuleRelativePath", "Public/GodotExportLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Export the whole Source Content Path from plugin settings." },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Function ExportAllToGodot constinit property declarations **********************
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ExportAllToGodot constinit property declarations ************************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ExportAllToGodot Property Definitions *********************************
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportAllToGodot_Parms, ReturnValue), Z_Construct_UScriptStruct_FGodotExportResult, METADATA_PARAMS(0, nullptr) }; // 8b762825eb434986cc7f1998bba6b795a03efbc9
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function ExportAllToGodot Property Definitions ***********************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGodotExportLibrary, nullptr, "ExportAllToGodot", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GodotExportLibrary_eventExportAllToGodot_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GodotExportLibrary_eventExportAllToGodot_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGodotExportLibrary_ExportAllToGodot(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGodotExportLibrary::execExportAllToGodot)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FGodotExportResult*)Z_Param__Result=UGodotExportLibrary::ExportAllToGodot();
	P_NATIVE_END;
}
// ********** End Class UGodotExportLibrary Function ExportAllToGodot ******************************

// ********** Begin Class UGodotExportLibrary Function ExportAssetsToGodot *************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGodotExportLibrary_ExportAssetsToGodot_Statics
struct UHT_STATICS
{
	struct GodotExportLibrary_eventExportAssetsToGodot_Parms
	{
		TArray<UObject*> Assets;
		FString GodotProjectPath;
		FGodotExportResult ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Godot Export" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Export several loaded assets. */" },
#endif
		{ "DevelopmentOnly", "" },
		{ "ModuleRelativePath", "Public/GodotExportLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Export several loaded assets." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Assets_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GodotProjectPath_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function ExportAssetsToGodot constinit property declarations *******************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Assets_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_Assets;
	static const UECodeGen_Private::FStrPropertyParams NewProp_GodotProjectPath;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ExportAssetsToGodot constinit property declarations *********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ExportAssetsToGodot Property Definitions ******************************
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_Assets_Inner = { "Assets", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Object, nullptr, nullptr, 1, 0, Z_Construct_UClass_UObject, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_Assets = { "Assets", nullptr, (EPropertyFlags)0x0010000008000182, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportAssetsToGodot_Parms, Assets), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Assets_MetaData), NewProp_Assets_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_GodotProjectPath = { "GodotProjectPath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportAssetsToGodot_Parms, GodotProjectPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GodotProjectPath_MetaData), NewProp_GodotProjectPath_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportAssetsToGodot_Parms, ReturnValue), Z_Construct_UScriptStruct_FGodotExportResult, METADATA_PARAMS(0, nullptr) }; // 8b762825eb434986cc7f1998bba6b795a03efbc9
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Assets_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Assets,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GodotProjectPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function ExportAssetsToGodot Property Definitions ********************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGodotExportLibrary, nullptr, "ExportAssetsToGodot", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GodotExportLibrary_eventExportAssetsToGodot_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04422401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GodotExportLibrary_eventExportAssetsToGodot_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGodotExportLibrary_ExportAssetsToGodot(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGodotExportLibrary::execExportAssetsToGodot)
{
	P_GET_TARRAY_REF(UObject*,Z_Param_Out_Assets);
	P_GET_PROPERTY(FStrProperty,Z_Param_GodotProjectPath);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FGodotExportResult*)Z_Param__Result=UGodotExportLibrary::ExportAssetsToGodot(Z_Param_Out_Assets,Z_Param_GodotProjectPath);
	P_NATIVE_END;
}
// ********** End Class UGodotExportLibrary Function ExportAssetsToGodot ***************************

// ********** Begin Class UGodotExportLibrary Function ExportAssetToGodot **************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGodotExportLibrary_ExportAssetToGodot_Statics
struct UHT_STATICS
{
	struct GodotExportLibrary_eventExportAssetToGodot_Parms
	{
		UObject* Asset;
		FString GodotProjectPath;
		FGodotExportResult ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Godot Export" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Export a single loaded asset into the configured Godot project (or the given path). */" },
#endif
		{ "DevelopmentOnly", "" },
		{ "ModuleRelativePath", "Public/GodotExportLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Export a single loaded asset into the configured Godot project (or the given path)." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GodotProjectPath_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function ExportAssetToGodot constinit property declarations ********************
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Asset;
	static const UECodeGen_Private::FStrPropertyParams NewProp_GodotProjectPath;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ExportAssetToGodot constinit property declarations **********************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ExportAssetToGodot Property Definitions *******************************
const UECodeGen_Private::FObjectPropertyParams UHT_STATICS::NewProp_Asset = { "Asset", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportAssetToGodot_Parms, Asset), Z_Construct_UClass_UObject, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_GodotProjectPath = { "GodotProjectPath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportAssetToGodot_Parms, GodotProjectPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GodotProjectPath_MetaData), NewProp_GodotProjectPath_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportAssetToGodot_Parms, ReturnValue), Z_Construct_UScriptStruct_FGodotExportResult, METADATA_PARAMS(0, nullptr) }; // 8b762825eb434986cc7f1998bba6b795a03efbc9
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Asset,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GodotProjectPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function ExportAssetToGodot Property Definitions *********************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGodotExportLibrary, nullptr, "ExportAssetToGodot", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GodotExportLibrary_eventExportAssetToGodot_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GodotExportLibrary_eventExportAssetToGodot_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGodotExportLibrary_ExportAssetToGodot(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGodotExportLibrary::execExportAssetToGodot)
{
	P_GET_OBJECT(UObject,Z_Param_Asset);
	P_GET_PROPERTY(FStrProperty,Z_Param_GodotProjectPath);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FGodotExportResult*)Z_Param__Result=UGodotExportLibrary::ExportAssetToGodot(Z_Param_Asset,Z_Param_GodotProjectPath);
	P_NATIVE_END;
}
// ********** End Class UGodotExportLibrary Function ExportAssetToGodot ****************************

// ********** Begin Class UGodotExportLibrary Function ExportContentFolderToGodot ******************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UFunction_UGodotExportLibrary_ExportContentFolderToGodot_Statics
struct UHT_STATICS
{
	struct GodotExportLibrary_eventExportContentFolderToGodot_Parms
	{
		FString UnrealContentPath;
		FString GodotProjectPath;
		FGodotExportResult ReturnValue;
	};
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "Category", "Godot Export" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Recursively export every supported asset under an Unreal content path, e.g. /Game/InfimaGames. */" },
#endif
		{ "DevelopmentOnly", "" },
		{ "ModuleRelativePath", "Public/GodotExportLibrary.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Recursively export every supported asset under an Unreal content path, e.g. /Game/InfimaGames." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_UnrealContentPath_MetaData[] = {
		{ "NativeConst", "" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_GodotProjectPath_MetaData[] = {
		{ "NativeConst", "" },
	};
#endif // WITH_METADATA

// ********** Begin Function ExportContentFolderToGodot constinit property declarations ************
	static const UECodeGen_Private::FStrPropertyParams NewProp_UnrealContentPath;
	static const UECodeGen_Private::FStrPropertyParams NewProp_GodotProjectPath;
	static const UECodeGen_Private::FStructPropertyParams NewProp_ReturnValue;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End Function ExportContentFolderToGodot constinit property declarations **************
	static const UECodeGen_Private::FFunctionParams FuncParams;
};

// ********** Begin Function ExportContentFolderToGodot Property Definitions ***********************
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_UnrealContentPath = { "UnrealContentPath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportContentFolderToGodot_Parms, UnrealContentPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_UnrealContentPath_MetaData), NewProp_UnrealContentPath_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_GodotProjectPath = { "GodotProjectPath", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportContentFolderToGodot_Parms, GodotProjectPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_GodotProjectPath_MetaData), NewProp_GodotProjectPath_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, STRUCT_OFFSET(GodotExportLibrary_eventExportContentFolderToGodot_Parms, ReturnValue), Z_Construct_UScriptStruct_FGodotExportResult, METADATA_PARAMS(0, nullptr) }; // 8b762825eb434986cc7f1998bba6b795a03efbc9
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_UnrealContentPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_GodotProjectPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_ReturnValue,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End Function ExportContentFolderToGodot Property Definitions *************************
const UECodeGen_Private::FFunctionParams UHT_STATICS::FuncParams = { { (FTypeConstructFunc*)Z_Construct_UClass_UGodotExportLibrary, nullptr, "ExportContentFolderToGodot", UHT_STATICS::PropPointers, UE_ARRAY_COUNT(UHT_STATICS::PropPointers), DataSizeOf<UHT_STATICS::GodotExportLibrary_eventExportContentFolderToGodot_Parms>(), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)},  };
static_assert(sizeof(UHT_STATICS::GodotExportLibrary_eventExportContentFolderToGodot_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UGodotExportLibrary_ExportContentFolderToGodot(ETypeConstructPhase Phase)
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, UHT_STATICS::FuncParams);
	}
	return ReturnFunction;
}
#undef UHT_STATICS
DEFINE_FUNCTION(UGodotExportLibrary::execExportContentFolderToGodot)
{
	P_GET_PROPERTY(FStrProperty,Z_Param_UnrealContentPath);
	P_GET_PROPERTY(FStrProperty,Z_Param_GodotProjectPath);
	P_FINISH;
	P_NATIVE_BEGIN;
	*(FGodotExportResult*)Z_Param__Result=UGodotExportLibrary::ExportContentFolderToGodot(Z_Param_UnrealContentPath,Z_Param_GodotProjectPath);
	P_NATIVE_END;
}
// ********** End Class UGodotExportLibrary Function ExportContentFolderToGodot ********************

// ********** Begin Class UGodotExportLibrary ******************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UGodotExportLibrary_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "IncludePath", "GodotExportLibrary.h" },
		{ "ModuleRelativePath", "Public/GodotExportLibrary.h" },
	};
#endif // WITH_METADATA

// ********** Begin Class UGodotExportLibrary constinit property declarations **********************
// ********** End Class UGodotExportLibrary constinit property declarations ************************
	static constexpr UE::CodeGen::FClassNativeFunction Funcs[] = {
		{ .NameUTF8 = UTF8TEXT("ExportAllToGodot"), .Pointer = &UGodotExportLibrary::execExportAllToGodot },
		{ .NameUTF8 = UTF8TEXT("ExportAssetsToGodot"), .Pointer = &UGodotExportLibrary::execExportAssetsToGodot },
		{ .NameUTF8 = UTF8TEXT("ExportAssetToGodot"), .Pointer = &UGodotExportLibrary::execExportAssetToGodot },
		{ .NameUTF8 = UTF8TEXT("ExportContentFolderToGodot"), .Pointer = &UGodotExportLibrary::execExportContentFolderToGodot },
	};
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UGodotExportLibrary_ExportAllToGodot, "ExportAllToGodot" }, // 12f63521f119389ceb5b4f9d152b9b7277c8cd62
		{ &Z_Construct_UFunction_UGodotExportLibrary_ExportAssetsToGodot, "ExportAssetsToGodot" }, // 3b06cfb9e403bd5be75091c278464747b1c1fc91
		{ &Z_Construct_UFunction_UGodotExportLibrary_ExportAssetToGodot, "ExportAssetToGodot" }, // 3c83cb857f8ce3ea4ae934dad8aa87cce123cbe2
		{ &Z_Construct_UFunction_UGodotExportLibrary_ExportContentFolderToGodot, "ExportContentFolderToGodot" }, // f3d3f9aaa9f5eca83fced1a884e31ee068a2193a
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGodotExportLibrary>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_UBlueprintFunctionLibrary,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GodotExporter,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UGodotExportLibrary,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static void UGodotExportLibrary_StaticRegisterNativesUGodotExportLibrary()
{
	UClass* Class = UGodotExportLibrary::StaticClass();
	FNativeFunctionRegistrar::RegisterFunctions(Class, 		MakeConstArrayView(UHT_STATICS::Funcs));
}
FClassRegistrationInfo Z_Registration_Info_UClass_UGodotExportLibrary;
UClass* Z_Construct_UClass_UGodotExportLibrary(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UGodotExportLibrary;
		if (!Z_Registration_Info_UClass_UGodotExportLibrary.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("GodotExportLibrary"),
				Z_Registration_Info_UClass_UGodotExportLibrary.InnerSingleton,
				UGodotExportLibrary_StaticRegisterNativesUGodotExportLibrary,
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
		return Z_Registration_Info_UClass_UGodotExportLibrary.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UGodotExportLibrary.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGodotExportLibrary.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UGodotExportLibrary.OuterSingleton;
}
#undef UHT_STATICS
UGodotExportLibrary::UGodotExportLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGodotExportLibrary);
UGodotExportLibrary::~UGodotExportLibrary() {}
// ********** End Class UGodotExportLibrary ********************************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportLibrary_h__Script_GodotExporter_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGodotExportLibrary, TEXT("UGodotExportLibrary"), &Z_Registration_Info_UClass_UGodotExportLibrary, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGodotExportLibrary), 513351633U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportLibrary_h__Script_GodotExporter_1eb4e1d563b810846619445f627dc370c619a71d{
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
