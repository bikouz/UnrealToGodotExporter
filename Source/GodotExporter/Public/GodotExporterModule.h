#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FGodotExporterModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FGodotExporterModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FGodotExporterModule>("GodotExporter");
	}

	void OpenExportWindow();
	void ExportSelectedAssets();
	void ExportSelectedFolders();

private:
	void RegisterMenus();
	void UnregisterMenus();
	void OnPackageSaved(const FString& PackageFilename, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);

	TSharedRef<class SDockTab> SpawnExportTab(const class FSpawnTabArgs& Args);

	FDelegateHandle PackageSavedHandle;
	bool bIsAutoExporting = false;
};
