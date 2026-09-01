// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GodotExportTypes.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGodotExportTypes() {}

// ********** Begin Cross Module References ********************************************************
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase);
GODOTEXPORTER_API UEnum* Z_Construct_UEnum_GodotExporter_EGodotExportStatus(ETypeConstructPhase);
GODOTEXPORTER_API UScriptStruct* Z_Construct_UScriptStruct_FGodotExportItemResult(ETypeConstructPhase);
GODOTEXPORTER_API UScriptStruct* Z_Construct_UScriptStruct_FGodotExportResult(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Enum EGodotExportStatus ********************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UEnum_GodotExporter_EGodotExportStatus_Statics
template<> GODOTEXPORTER_NON_ATTRIBUTED_API UEnum* StaticEnum<EGodotExportStatus>()
{
	return Z_Construct_UEnum_GodotExporter_EGodotExportStatus(ETypeConstructPhase::Outer);
}
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "Failed.Name", "EGodotExportStatus::Failed" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
		{ "Skipped.Name", "EGodotExportStatus::Skipped" },
		{ "Succeeded.Name", "EGodotExportStatus::Succeeded" },
		{ "Unsupported.Name", "EGodotExportStatus::Unsupported" },
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EGodotExportStatus::Succeeded", (int64)EGodotExportStatus::Succeeded },
		{ "EGodotExportStatus::Failed", (int64)EGodotExportStatus::Failed },
		{ "EGodotExportStatus::Skipped", (int64)EGodotExportStatus::Skipped },
		{ "EGodotExportStatus::Unsupported", (int64)EGodotExportStatus::Unsupported },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
}; // struct UHT_STATICS 
const UECodeGen_Private::FEnumParams UHT_STATICS::EnumParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GodotExporter,
	nullptr,
	"EGodotExportStatus",
	"EGodotExportStatus",
	UHT_STATICS::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(UHT_STATICS::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	(uint8)UEnum::EUnderlyingType::uint8,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FEnumRegistrationInfo ZRIE_EGodotExportStatus;
UEnum* Z_Construct_UEnum_GodotExporter_EGodotExportStatus(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!ZRIE_EGodotExportStatus.OuterSingleton)
		{
			ZRIE_EGodotExportStatus.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GodotExporter_EGodotExportStatus, (UObject*)Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase::Outer), TEXT("EGodotExportStatus"));
		}
		return ZRIE_EGodotExportStatus.OuterSingleton;
	}
	if (!ZRIE_EGodotExportStatus.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(ZRIE_EGodotExportStatus.InnerSingleton, UHT_STATICS::EnumParams);
	}
	return ZRIE_EGodotExportStatus.InnerSingleton;
}
#undef UHT_STATICS
// ********** End Enum EGodotExportStatus **********************************************************

// ********** Begin ScriptStruct FGodotExportItemResult ********************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FGodotExportItemResult_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FGodotExportItemResult>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FGodotExportItemResult); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_AssetPath_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_OutputPath_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Status_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Message_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FGodotExportItemResult constinit property declarations ************
	static const UECodeGen_Private::FStrPropertyParams NewProp_AssetPath;
	static const UECodeGen_Private::FStrPropertyParams NewProp_OutputPath;
	static const UECodeGen_Private::FBytePropertyParams NewProp_Status_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_Status;
	static const UECodeGen_Private::FStrPropertyParams NewProp_Message;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FGodotExportItemResult constinit property declarations **************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGodotExportItemResult>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FGodotExportItemResult Property Definitions ***********************
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_AssetPath = { "AssetPath", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportItemResult, AssetPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_AssetPath_MetaData), NewProp_AssetPath_MetaData) };
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_OutputPath = { "OutputPath", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportItemResult, OutputPath), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_OutputPath_MetaData), NewProp_OutputPath_MetaData) };
const UECodeGen_Private::FBytePropertyParams UHT_STATICS::NewProp_Status_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams UHT_STATICS::NewProp_Status = { "Status", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Enum, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportItemResult, Status), Z_Construct_UEnum_GodotExporter_EGodotExportStatus, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Status_MetaData), NewProp_Status_MetaData) }; // 83d977b667b41dde05f6da60f3aa7879b129c364
const UECodeGen_Private::FStrPropertyParams UHT_STATICS::NewProp_Message = { "Message", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Str, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportItemResult, Message), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Message_MetaData), NewProp_Message_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_AssetPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_OutputPath,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Status_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Status,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Message,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FGodotExportItemResult Property Definitions *************************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GodotExporter,
	nullptr,
	&NewStructOps,
	"GodotExportItemResult",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FGodotExportItemResult>(),
	alignof(FGodotExportItemResult),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000201),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FGodotExportItemResult;
UScriptStruct* Z_Construct_UScriptStruct_FGodotExportItemResult(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FGodotExportItemResult.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FGodotExportItemResult.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FGodotExportItemResult, (UObject*)Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase::Outer), TEXT("GodotExportItemResult"));
		}
		return Z_Registration_Info_UScriptStruct_FGodotExportItemResult.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FGodotExportItemResult.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FGodotExportItemResult.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FGodotExportItemResult.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FGodotExportItemResult **********************************************

// ********** Begin ScriptStruct FGodotExportResult ************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UScriptStruct_FGodotExportResult_Statics
struct UHT_STATICS
{
	static inline consteval int32 GetStructSize() { return DataSizeOf<FGodotExportResult>(); }
	static inline consteval int16 GetStructAlignment() { return alignof(FGodotExportResult); }
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NumSucceeded_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NumFailed_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NumSkipped_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_NumUnsupported_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Items_MetaData[] = {
		{ "Category", "Godot Export" },
		{ "ModuleRelativePath", "Public/GodotExportTypes.h" },
	};
#endif // WITH_METADATA

// ********** Begin ScriptStruct FGodotExportResult constinit property declarations ****************
	static const UECodeGen_Private::FIntPropertyParams NewProp_NumSucceeded;
	static const UECodeGen_Private::FIntPropertyParams NewProp_NumFailed;
	static const UECodeGen_Private::FIntPropertyParams NewProp_NumSkipped;
	static const UECodeGen_Private::FIntPropertyParams NewProp_NumUnsupported;
	static const UECodeGen_Private::FStructPropertyParams NewProp_Items_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_Items;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
// ********** End ScriptStruct FGodotExportResult constinit property declarations ******************
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FGodotExportResult>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
}; // struct UHT_STATICS

// ********** Begin ScriptStruct FGodotExportResult Property Definitions ***************************
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_NumSucceeded = { "NumSucceeded", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportResult, NumSucceeded), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NumSucceeded_MetaData), NewProp_NumSucceeded_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_NumFailed = { "NumFailed", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportResult, NumFailed), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NumFailed_MetaData), NewProp_NumFailed_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_NumSkipped = { "NumSkipped", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportResult, NumSkipped), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NumSkipped_MetaData), NewProp_NumSkipped_MetaData) };
const UECodeGen_Private::FIntPropertyParams UHT_STATICS::NewProp_NumUnsupported = { "NumUnsupported", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Int, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportResult, NumUnsupported), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_NumUnsupported_MetaData), NewProp_NumUnsupported_MetaData) };
const UECodeGen_Private::FStructPropertyParams UHT_STATICS::NewProp_Items_Inner = { "Items", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FGodotExportItemResult, METADATA_PARAMS(0, nullptr) }; // 59a3f414b0c46e1e45b8bbed19bac6bc3cc14ed0
const UECodeGen_Private::FArrayPropertyParams UHT_STATICS::NewProp_Items = { "Items", nullptr, (EPropertyFlags)0x0010000000000014, UECodeGen_Private::EPropertyGenFlags::Array, nullptr, nullptr, 1, STRUCT_OFFSET(FGodotExportResult, Items), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Items_MetaData), NewProp_Items_MetaData) }; // 59a3f414b0c46e1e45b8bbed19bac6bc3cc14ed0
const UECodeGen_Private::FPropertyParamsBase* const UHT_STATICS::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_NumSucceeded,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_NumFailed,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_NumSkipped,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_NumUnsupported,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Items_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&UHT_STATICS::NewProp_Items,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::PropPointers) < 2048);
// ********** End ScriptStruct FGodotExportResult Property Definitions *****************************
const UECodeGen_Private::FStructParams UHT_STATICS::StructParams = {
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GodotExporter,
	nullptr,
	&NewStructOps,
	"GodotExportResult",
	UHT_STATICS::PropPointers,
	UE_ARRAY_COUNT(UHT_STATICS::PropPointers),
	DataSizeOf<FGodotExportResult>(),
	alignof(FGodotExportResult),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000201),
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FGodotExportResult;
UScriptStruct* Z_Construct_UScriptStruct_FGodotExportResult(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Outer)
	{
		if (!Z_Registration_Info_UScriptStruct_FGodotExportResult.OuterSingleton)
		{
			Z_Registration_Info_UScriptStruct_FGodotExportResult.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FGodotExportResult, (UObject*)Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase::Outer), TEXT("GodotExportResult"));
		}
		return Z_Registration_Info_UScriptStruct_FGodotExportResult.OuterSingleton;
	}
	if (!Z_Registration_Info_UScriptStruct_FGodotExportResult.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FGodotExportResult.InnerSingleton, UHT_STATICS::StructParams);
	}
	return CastChecked<UScriptStruct>(Z_Registration_Info_UScriptStruct_FGodotExportResult.InnerSingleton);
}
#undef UHT_STATICS
// ********** End ScriptStruct FGodotExportResult **************************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportTypes_h__Script_GodotExporter_Statics
struct UHT_STATICS
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ Z_Construct_UEnum_GodotExporter_EGodotExportStatus, TEXT("EGodotExportStatus"), &ZRIE_EGodotExportStatus, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2212067254U) },
	};
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ Z_Construct_UScriptStruct_FGodotExportItemResult, Z_Construct_UScriptStruct_FGodotExportItemResult_Statics::NewStructOps, TEXT("GodotExportItemResult"),&Z_Registration_Info_UScriptStruct_FGodotExportItemResult, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FGodotExportItemResult), 1503917076U) },
		{ Z_Construct_UScriptStruct_FGodotExportResult, Z_Construct_UScriptStruct_FGodotExportResult_Statics::NewStructOps, TEXT("GodotExportResult"),&Z_Registration_Info_UScriptStruct_FGodotExportResult, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FGodotExportResult), 2339776549U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportTypes_h__Script_GodotExporter_85d3788b6314863423ee6d335f735e7060d581b7{
	TEXT("/Script/GodotExporter"),
	nullptr, 0,
	UHT_STATICS::ScriptStructInfo, UE_ARRAY_COUNT(UHT_STATICS::ScriptStructInfo),
	UHT_STATICS::EnumInfo, UE_ARRAY_COUNT(UHT_STATICS::EnumInfo),
	nullptr, 0,
};
#undef UHT_STATICS
// ********** End Registration *********************************************************************
#undef UHT_STRUCT_BASE

PRAGMA_ENABLE_DEPRECATION_WARNINGS
