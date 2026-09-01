// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGodotExporter_init() {}
static_assert(!UE_WITH_CONSTINIT_UOBJECT, "This generated code can only be compiled with !UE_WITH_CONSTINIT_OBJECT");
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_GodotExporter;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_GodotExporter(ETypeConstructPhase)
	{
		if (!Z_Registration_Info_UPackage__Script_GodotExporter.OuterSingleton)
		{
		static const UECodeGen_Private::FPackageParams PackageParams = {
			"/Script/GodotExporter",
			nullptr,
			0,
			PKG_CompiledIn | 0x00000040,
			0x83DFA366,
			0xCE10F44D,
			METADATA_PARAMS(0, nullptr)
		};
		UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_GodotExporter.OuterSingleton, PackageParams);
	}
	return Z_Registration_Info_UPackage__Script_GodotExporter.OuterSingleton;
}
static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_GodotExporter(Z_Construct_UPackage__Script_GodotExporter, TEXT("/Script/GodotExporter"), Z_Registration_Info_UPackage__Script_GodotExporter, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0x83DFA366, 0xCE10F44D));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
