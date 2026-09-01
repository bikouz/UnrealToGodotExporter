// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "GodotExportTypes.h"

#ifdef GODOTEXPORTER_GodotExportTypes_generated_h
#error "GodotExportTypes.generated.h already included, missing '#pragma once' in GodotExportTypes.h"
#endif
#define GODOTEXPORTER_GodotExportTypes_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FGodotExportItemResult ********************************************
struct Z_Construct_UScriptStruct_FGodotExportItemResult_Statics;
GODOTEXPORTER_API UScriptStruct* Z_Construct_UScriptStruct_FGodotExportItemResult(ETypeConstructPhase);

#define FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportTypes_h_18_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FGodotExportItemResult_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FGodotExportItemResult(ETypeConstructPhase::Inner); }


struct FGodotExportItemResult;
// ********** End ScriptStruct FGodotExportItemResult **********************************************

// ********** Begin ScriptStruct FGodotExportResult ************************************************
struct Z_Construct_UScriptStruct_FGodotExportResult_Statics;
GODOTEXPORTER_API UScriptStruct* Z_Construct_UScriptStruct_FGodotExportResult(ETypeConstructPhase);

#define FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportTypes_h_36_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FGodotExportResult_Statics; \
	UE_NODEBUG static UScriptStruct* StaticStruct() { return Z_Construct_UScriptStruct_FGodotExportResult(ETypeConstructPhase::Inner); }


struct FGodotExportResult;
// ********** End ScriptStruct FGodotExportResult **************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_BIKOUZ_PC_Documents_Unreal_Projects_Downgrader_Plugins_GodotExporter_Source_GodotExporter_Public_GodotExportTypes_h

// ********** Begin Enum EGodotExportStatus ********************************************************
#define FOREACH_ENUM_EGODOTEXPORTSTATUS(op) \
	op(EGodotExportStatus::Succeeded) \
	op(EGodotExportStatus::Failed) \
	op(EGodotExportStatus::Skipped) \
	op(EGodotExportStatus::Unsupported) 

enum class EGodotExportStatus : uint8;
template<> struct TIsUEnumClass<EGodotExportStatus> { enum { Value = true }; };
template<> UE_NODEBUG GODOTEXPORTER_NON_ATTRIBUTED_API UEnum* StaticEnum<EGodotExportStatus>();
// ********** End Enum EGodotExportStatus **********************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
