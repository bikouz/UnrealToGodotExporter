#pragma once

#include "CoreMinimal.h"
#include "GodotExportTypes.generated.h"

UENUM(BlueprintType)
enum class EGodotExportStatus : uint8
{
	Succeeded,
	Failed,
	Skipped,
	Unsupported
};

USTRUCT(BlueprintType)
struct GODOTEXPORTER_API FGodotExportItemResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	FString AssetPath;

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	FString OutputPath;

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	EGodotExportStatus Status = EGodotExportStatus::Failed;

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	FString Message;
};

USTRUCT(BlueprintType)
struct GODOTEXPORTER_API FGodotExportResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	int32 NumSucceeded = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	int32 NumFailed = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	int32 NumSkipped = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	int32 NumUnsupported = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Godot Export")
	TArray<FGodotExportItemResult> Items;

	FString ToSummary() const
	{
		return FString::Printf(
			TEXT("Godot export finished: %d succeeded, %d skipped, %d unsupported, %d failed"),
			NumSucceeded,
			NumSkipped,
			NumUnsupported,
			NumFailed);
	}
};
