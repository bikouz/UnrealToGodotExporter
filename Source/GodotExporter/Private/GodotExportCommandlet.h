#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GodotExportCommandlet.generated.h"

/**
 * Headless export.
 * UnrealEditor-Cmd.exe Downgrader.uproject -run=GodotExport -GodotProject="C:/Godot/MyGame" -Path=/Game
 */
UCLASS()
class UGodotExportCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UGodotExportCommandlet();
	virtual int32 Main(const FString& Params) override;
};
