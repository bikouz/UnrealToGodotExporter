#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GodotExportTypes.h"
#include "GodotExportLibrary.generated.h"

UCLASS()
class GODOTEXPORTER_API UGodotExportLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Export a single loaded asset into the configured Godot project (or the given path). */
	UFUNCTION(BlueprintCallable, Category = "Godot Export", meta = (DevelopmentOnly))
	static FGodotExportResult ExportAssetToGodot(UObject* Asset, const FString& GodotProjectPath);

	/** Export several loaded assets. */
	UFUNCTION(BlueprintCallable, Category = "Godot Export", meta = (DevelopmentOnly))
	static FGodotExportResult ExportAssetsToGodot(const TArray<UObject*>& Assets, const FString& GodotProjectPath);

	/** Recursively export every supported asset under an Unreal content path, e.g. /Game/InfimaGames. */
	UFUNCTION(BlueprintCallable, Category = "Godot Export", meta = (DevelopmentOnly))
	static FGodotExportResult ExportContentFolderToGodot(const FString& UnrealContentPath, const FString& GodotProjectPath);

	/** Export the whole Source Content Path from plugin settings. */
	UFUNCTION(BlueprintCallable, Category = "Godot Export", meta = (DevelopmentOnly))
	static FGodotExportResult ExportAllToGodot();
};
