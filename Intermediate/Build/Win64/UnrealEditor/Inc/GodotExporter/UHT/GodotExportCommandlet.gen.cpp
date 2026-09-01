// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GodotExportCommandlet.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_UOBJECT");
void EmptyLinkFunctionForGeneratedCodeGodotExportCommandlet() {}

// ********** Begin Cross Module References ********************************************************
ENGINE_API UClass* Z_Construct_UClass_UCommandlet(ETypeConstructPhase);
// ********** End Cross Module References **********************************************************

// ********** Begin Same Module References *********************************************************
UPackage* Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase);
GODOTEXPORTER_API UClass* Z_Construct_UClass_UGodotExportCommandlet(ETypeConstructPhase);
GODOTEXPORTER_API UClass* Z_Construct_UClass_UGodotExportCommandlet(ETypeConstructPhase);
// ********** End Same Module References ***********************************************************
#define UHT_STRUCT_BASE(INIT) UE::CodeGen::ConstInit::TCompiledInObjectPtr<const FStructBaseChain>(UE::Private::AsStructBaseChain(INIT))

// ********** Begin Class UGodotExportCommandlet ***************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_Construct_UClass_UGodotExportCommandlet_Statics
struct UHT_STATICS
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Type_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Headless export.\n * UnrealEditor-Cmd.exe Downgrader.uproject -run=GodotExport -GodotProject=\"C:/Godot/MyGame\" -Path=/Game\n */" },
#endif
		{ "IncludePath", "GodotExportCommandlet.h" },
		{ "ModuleRelativePath", "Private/GodotExportCommandlet.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Headless export.\nUnrealEditor-Cmd.exe Downgrader.uproject -run=GodotExport -GodotProject=\"C:/Godot/MyGame\" -Path=/Game" },
#endif
	};
#endif // WITH_METADATA

// ********** Begin Class UGodotExportCommandlet constinit property declarations *******************
// ********** End Class UGodotExportCommandlet constinit property declarations *********************
	static FTypeConstructFunc* DependentSingletons[];
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGodotExportCommandlet>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
}; // struct UHT_STATICS
FTypeConstructFunc* UHT_STATICS::DependentSingletons[] = {
	(FTypeConstructFunc*)Z_Construct_UClass_UCommandlet,
	(FTypeConstructFunc*)Z_Construct_UPackage__Script_GodotExporter,
};
static_assert(UE_ARRAY_COUNT(UHT_STATICS::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams UHT_STATICS::ClassParams = {
	&Z_Construct_UClass_UGodotExportCommandlet,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x000000A8u,
	METADATA_PARAMS(UE_ARRAY_COUNT(UHT_STATICS::Type_MetaData), UHT_STATICS::Type_MetaData)
};
FClassRegistrationInfo Z_Registration_Info_UClass_UGodotExportCommandlet;
UClass* Z_Construct_UClass_UGodotExportCommandlet(ETypeConstructPhase Phase)
{
	if (Phase == ETypeConstructPhase::Inner)
	{
		using TClass = UGodotExportCommandlet;
		if (!Z_Registration_Info_UClass_UGodotExportCommandlet.InnerSingleton)
		{
			GetPrivateStaticClassBody(
				TClass::StaticPackage(),
				TEXT("GodotExportCommandlet"),
				Z_Registration_Info_UClass_UGodotExportCommandlet.InnerSingleton,
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
		return Z_Registration_Info_UClass_UGodotExportCommandlet.InnerSingleton;
	}
	if (!Z_Registration_Info_UClass_UGodotExportCommandlet.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGodotExportCommandlet.OuterSingleton, UHT_STATICS::ClassParams);
	}
	return Z_Registration_Info_UClass_UGodotExportCommandlet.OuterSingleton;
}
#undef UHT_STATICS
DEFINE_VTABLE_PTR_HELPER_CTOR_NS(, UGodotExportCommandlet);
UGodotExportCommandlet::~UGodotExportCommandlet() {}
// ********** End Class UGodotExportCommandlet *****************************************************

// ********** Begin Registration *******************************************************************
#ifdef UHT_STATICS
#error UHT_STATICS already defined
#endif
#define UHT_STATICS Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Private_GodotExportCommandlet_h__Script_GodotExporter_Statics
struct UHT_STATICS
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UGodotExportCommandlet, TEXT("UGodotExportCommandlet"), &Z_Registration_Info_UClass_UGodotExportCommandlet, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGodotExportCommandlet), 3592147175U) },
	};
}; // UHT_STATICS 
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Private_GodotExportCommandlet_h__Script_GodotExporter_3ecb5ae904dbcc5d5dd696886376b1e12faeaec0{
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
