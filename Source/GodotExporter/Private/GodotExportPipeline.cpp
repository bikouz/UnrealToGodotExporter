#include "GodotExportPipeline.h"

#include "Animation/AnimSequence.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "DesktopPlatformModule.h"
#include "EditorDirectories.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/DataTable.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"
#include "Engine/World.h"
#include "Exporters/GLTFExporter.h"
#include "Framework/Application/SlateApplication.h"
#include "GodotExportSettings.h"
#include "HAL/FileManager.h"
#include "ImageCore.h"
#include "ImageUtils.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Options/GLTFExportOptions.h"
#include "Sound/SoundWave.h"
#include "UObject/Package.h"
#include "Widgets/SWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogGodotExporter, Log, All);

namespace GodotExportPrivate
{
	FString SanitizePathSegment(const FString& In, bool bReplaceSpaces)
	{
		FString Out = In;
		Out.ReplaceInline(TEXT("\\"), TEXT("/"));
		const TCHAR* Invalid = TEXT(":*?\"<>|");
		for (const TCHAR* C = Invalid; *C; ++C)
		{
			Out.ReplaceCharInline(*C, TEXT('_'));
		}
		if (bReplaceSpaces)
		{
			Out.ReplaceInline(TEXT(" "), TEXT("_"));
		}
		return Out;
	}

	bool IsEngineOrScriptPackage(const FString& PackageName)
	{
		return PackageName.StartsWith(TEXT("/Engine"))
			|| PackageName.StartsWith(TEXT("/Script"))
			|| PackageName.StartsWith(TEXT("/Temp"))
			|| PackageName.StartsWith(TEXT("/Memory"))
			|| PackageName.StartsWith(TEXT("/Config"));
	}

	bool IsAnimSequenceAsset(const FAssetData& Asset)
	{
		if (Asset.IsInstanceOf(UAnimSequence::StaticClass(), EResolveClass::Yes))
		{
			return true;
		}
		return Asset.AssetClassPath.GetAssetName() == TEXT("AnimSequence");
	}

	bool MatchesClass(const FAssetData& Asset, const UClass* Class)
	{
		if (!Class)
		{
			return false;
		}
		if (Asset.IsInstanceOf(Class, EResolveClass::Yes))
		{
			return true;
		}
		return Asset.AssetClassPath == Class->GetClassPathName()
			|| Asset.AssetClassPath.GetAssetName() == Class->GetFName();
	}

	bool IsSupportedClassForFilter(const FAssetData& Asset)
	{
		return MatchesClass(Asset, UStaticMesh::StaticClass())
			|| MatchesClass(Asset, USkeletalMesh::StaticClass())
			|| IsAnimSequenceAsset(Asset)
			|| MatchesClass(Asset, UTexture2D::StaticClass())
			|| MatchesClass(Asset, UMaterialInterface::StaticClass())
			|| MatchesClass(Asset, USoundWave::StaticClass())
			|| MatchesClass(Asset, UWorld::StaticClass())
			|| MatchesClass(Asset, UDataTable::StaticClass());
	}

	bool SelectionIsAnimSequenceOnly(const TArray<FAssetData>& Assets)
	{
		bool bHasAnim = false;
		for (const FAssetData& Asset : Assets)
		{
			if (!Asset.IsValid() || Asset.IsRedirector())
			{
				continue;
			}
			if (IsAnimSequenceAsset(Asset))
			{
				bHasAnim = true;
				continue;
			}
			if (IsSupportedClassForFilter(Asset))
			{
				return false;
			}
		}
		return bHasAnim;
	}

	FString TypeFolderForExtension(const FString& Extension)
	{
		const FString E = Extension.ToLower();
		if (E == TEXT("png") || E == TEXT("jpg") || E == TEXT("jpeg") || E == TEXT("tga") || E == TEXT("webp") || E == TEXT("exr"))
		{
			return TEXT("textures");
		}
		if (E == TEXT("tres"))
		{
			return TEXT("materials");
		}
		if (E == TEXT("glb") || E == TEXT("gltf"))
		{
			return TEXT("meshes");
		}
		if (E == TEXT("wav") || E == TEXT("ogg") || E == TEXT("mp3"))
		{
			return TEXT("audio");
		}
		if (E == TEXT("json"))
		{
			return TEXT("data");
		}
		return TEXT("other");
	}

	FString TypeFolderForAsset(const FAssetData& Asset, const FString& Extension)
	{
		if (IsAnimSequenceAsset(Asset))
		{
			return TEXT("anims");
		}
		return TypeFolderForExtension(Extension);
	}

	bool ShouldPullDependencies(const FAssetData& Asset)
	{
		return MatchesClass(Asset, UWorld::StaticClass());
	}

	FString ContentBrowserFolderToPackagePath(const FString& Folder)
	{
		FString Path = Folder;
		Path.ReplaceInline(TEXT("\\"), TEXT("/"));
		if (Path.StartsWith(TEXT("/All/Game")))
		{
			Path = FString(TEXT("/Game")) + Path.Mid(9);
		}
		else if (Path.StartsWith(TEXT("/All")))
		{
			Path = Path.Mid(4);
		}
		return Path;
	}

	int32 TypePriority(const UClass* Class)
	{
		if (!Class)
		{
			return 100;
		}
		if (Class->IsChildOf(UTexture2D::StaticClass()))
		{
			return 0;
		}
		if (Class->IsChildOf(USoundWave::StaticClass()))
		{
			return 1;
		}
		if (Class->IsChildOf(UMaterialInterface::StaticClass()))
		{
			return 2;
		}
		if (Class->IsChildOf(UStaticMesh::StaticClass()) || Class->IsChildOf(USkeletalMesh::StaticClass()))
		{
			return 3;
		}
		if (Class->IsChildOf(UAnimSequence::StaticClass()))
		{
			return 4;
		}
		if (Class->IsChildOf(UDataTable::StaticClass()))
		{
			return 5;
		}
		if (Class->IsChildOf(UWorld::StaticClass()))
		{
			return 6;
		}
		return 50;
	}

	FString GuessTextureRole(const FString& Name)
	{
		const FString Lower = Name.ToLower();

		auto EndsWithToken = [&Lower](const TCHAR* Token)
		{
			return Lower.EndsWith(Token) || Lower.Contains(FString(Token) + TEXT(".")) || Lower.EndsWith(FString(Token) + TEXT("map"));
		};

		if (Lower.Contains(TEXT("normal")) || EndsWithToken(TEXT("_n")) || EndsWithToken(TEXT("_nrm")) || EndsWithToken(TEXT("_norm")))
		{
			return TEXT("normal");
		}
		if (Lower.Contains(TEXT("emissi")) || EndsWithToken(TEXT("_e")) || EndsWithToken(TEXT("_em")))
		{
			return TEXT("emission");
		}
		if (Lower.Contains(TEXT("orm")) || Lower.Contains(TEXT("_arm")) || Lower.Contains(TEXT("mra"))
			|| Lower.Contains(TEXT("_mask")) || EndsWithToken(TEXT("_orm")) || EndsWithToken(TEXT("_mra")) || EndsWithToken(TEXT("_arm")))
		{
			return TEXT("orm");
		}
		if (Lower.Contains(TEXT("rough")))
		{
			return TEXT("roughness");
		}
		if (Lower.Contains(TEXT("metal")))
		{
			return TEXT("metallic");
		}
		if (Lower.Contains(TEXT("ambientocclusion")) || Lower.Contains(TEXT("occlusion"))
			|| Lower.Contains(TEXT("_ao")) || EndsWithToken(TEXT("_ao")))
		{
			return TEXT("ao");
		}
		if (Lower.Contains(TEXT("opacity")) || Lower.Contains(TEXT("alphamask")))
		{
			return TEXT("alpha");
		}
		if (Lower.Contains(TEXT("basecolor")) || Lower.Contains(TEXT("base_color")) || Lower.Contains(TEXT("basecolour"))
			|| Lower.Contains(TEXT("albedo")) || Lower.Contains(TEXT("diffuse")) || Lower.Contains(TEXT("basemap"))
			|| Lower.Contains(TEXT("base_map")) || EndsWithToken(TEXT("_d")) || EndsWithToken(TEXT("_c"))
			|| EndsWithToken(TEXT("_bc")) || EndsWithToken(TEXT("_diff")) || EndsWithToken(TEXT("_col"))
			|| (Lower.Contains(TEXT("color")) && !Lower.Contains(TEXT("vertex"))))
		{
			return TEXT("albedo");
		}
		return TEXT("");
	}

	bool IsEngineTexture(const UTexture* Texture)
	{
		if (!Texture)
		{
			return true;
		}
		const FString Path = Texture->GetPathName();
		return Path.StartsWith(TEXT("/Engine")) || Path.StartsWith(TEXT("/Script"));
	}

	bool WriteWavFile(const FString& FilePath, const TArray<uint8>& PCM, uint32 SampleRate, uint16 NumChannels, uint16 BitsPerSample)
	{
		const uint32 DataSize = static_cast<uint32>(PCM.Num());
		const uint32 ByteRate = SampleRate * NumChannels * (BitsPerSample / 8);
		const uint16 BlockAlign = NumChannels * (BitsPerSample / 8);
		const uint32 FileSizeMinus8 = 36 + DataSize;

		TArray<uint8> Bytes;
		Bytes.Reserve(44 + PCM.Num());

		auto AppendString = [&Bytes](const char* S, int32 Count)
		{
			for (int32 i = 0; i < Count; ++i)
			{
				Bytes.Add(static_cast<uint8>(S[i]));
			}
		};
		auto AppendU16 = [&Bytes](uint16 V)
		{
			Bytes.Add(static_cast<uint8>(V & 0xFF));
			Bytes.Add(static_cast<uint8>((V >> 8) & 0xFF));
		};
		auto AppendU32 = [&Bytes](uint32 V)
		{
			Bytes.Add(static_cast<uint8>(V & 0xFF));
			Bytes.Add(static_cast<uint8>((V >> 8) & 0xFF));
			Bytes.Add(static_cast<uint8>((V >> 16) & 0xFF));
			Bytes.Add(static_cast<uint8>((V >> 24) & 0xFF));
		};

		AppendString("RIFF", 4);
		AppendU32(FileSizeMinus8);
		AppendString("WAVE", 4);
		AppendString("fmt ", 4);
		AppendU32(16);
		AppendU16(1);
		AppendU16(NumChannels);
		AppendU32(SampleRate);
		AppendU32(ByteRate);
		AppendU16(BlockAlign);
		AppendU16(BitsPerSample);
		AppendString("data", 4);
		AppendU32(DataSize);
		Bytes.Append(PCM);

		IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);
		return FFileHelper::SaveArrayToFile(Bytes, *FilePath);
	}

	FString ToResPathFromAbsolute(const FString& GodotProject, const FString& AbsoluteFile)
	{
		FString Relative = AbsoluteFile;
		FPaths::MakePathRelativeTo(Relative, *FPaths::ConvertRelativePathToFull(GodotProject + TEXT("/")));
		Relative.ReplaceInline(TEXT("\\"), TEXT("/"));
		return FString(TEXT("res://")) + Relative;
	}

	FString EscapeJson(FString Value)
	{
		Value.ReplaceInline(TEXT("\\"), TEXT("\\\\"));
		Value.ReplaceInline(TEXT("\""), TEXT("\\\""));
		Value.ReplaceInline(TEXT("\n"), TEXT("\\n"));
		Value.ReplaceInline(TEXT("\r"), TEXT("\\r"));
		Value.ReplaceInline(TEXT("\t"), TEXT("\\t"));
		return Value;
	}

	UAssetImportData* FindImportData(UObject* Asset)
	{
		if (UTexture* Texture = Cast<UTexture>(Asset))
		{
			return Texture->AssetImportData;
		}
		if (USoundWave* Sound = Cast<USoundWave>(Asset))
		{
			return Sound->AssetImportData;
		}
		if (UStaticMesh* Mesh = Cast<UStaticMesh>(Asset))
		{
			return Mesh->GetAssetImportData();
		}
		return nullptr;
	}
}

FGodotExportPipeline::FGodotExportPipeline(const UGodotExportSettings* InSettings, const FString& InOutputRoot, bool bInGodotProjectMode)
	: Settings(InSettings)
	, GodotProject(InOutputRoot)
	, bGodotProjectMode(bInGodotProjectMode)
{
}

FString FGodotExportPipeline::PackageToResPath(const FString& PackageName, const FString& Extension, const UGodotExportSettings* Settings, const FAssetData& Asset)
{
	FString Subfolder = (Settings && !Settings->ExportSubfolder.IsEmpty()) ? Settings->ExportSubfolder : TEXT("export");
	Subfolder.ReplaceInline(TEXT("\\"), TEXT("/"));
	Subfolder.RemoveFromStart(TEXT("/"));
	Subfolder.RemoveFromEnd(TEXT("/"));
	Subfolder = GodotExportPrivate::SanitizePathSegment(Subfolder, true);

	const FString TypeFolder = Asset.IsValid()
		? GodotExportPrivate::TypeFolderForAsset(Asset, Extension)
		: GodotExportPrivate::TypeFolderForExtension(Extension);
	FString FileName = FPackageName::GetShortName(PackageName);
	FileName = GodotExportPrivate::SanitizePathSegment(FileName, Settings ? Settings->bReplaceSpacesInPaths : true);
	if (!Extension.IsEmpty())
	{
		FileName += TEXT(".") + Extension;
	}

	FString Path = TypeFolder + TEXT("/") + FileName;
	if (!Subfolder.IsEmpty())
	{
		Path = Subfolder + TEXT("/") + Path;
	}
	return FString(TEXT("res://")) + Path;
}

FString FGodotExportPipeline::PackageToAbsolutePath(
	const FString& InGodotProject,
	const FString& PackageName,
	const FString& Extension,
	const UGodotExportSettings* Settings,
	const FAssetData& Asset)
{
	FString Res = PackageToResPath(PackageName, Extension, Settings, Asset);
	Res.RemoveFromStart(TEXT("res://"));
	return FPaths::Combine(InGodotProject, Res);
}

bool FGodotExportPipeline::PromptForGodotProject(FString& OutPath)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return false;
	}

	const void* ParentWindow = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(TSharedPtr<SWidget>());
	FString Folder;
	const bool bPicked = DesktopPlatform->OpenDirectoryDialog(
		ParentWindow,
		TEXT("Select Godot project folder"),
		OutPath,
		Folder);

	if (bPicked && !Folder.IsEmpty())
	{
		OutPath = Folder;
		return true;
	}
	return false;
}

bool FGodotExportPipeline::PromptForDirectory(const FString& Title, FString& OutPath)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return false;
	}

	const void* ParentWindow = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(TSharedPtr<SWidget>());
	FString Folder;
	FString Start = OutPath;
	if (Start.IsEmpty())
	{
		Start = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_EXPORT);
	}
	const bool bPicked = DesktopPlatform->OpenDirectoryDialog(ParentWindow, Title, Start, Folder);
	if (bPicked && !Folder.IsEmpty())
	{
		OutPath = Folder;
		FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_EXPORT, Folder);
		return true;
	}
	return false;
}

FString FGodotExportPipeline::ResolveGodotProjectPath(const UGodotExportSettings* Settings, bool bPromptIfMissing)
{
	FString Path = Settings ? Settings->GodotProjectPath.Path : FString();
	if (Path.IsEmpty() && bPromptIfMissing)
	{
		if (PromptForGodotProject(Path))
		{
			if (UGodotExportSettings* Mutable = GetMutableDefault<UGodotExportSettings>())
			{
				Mutable->GodotProjectPath.Path = Path;
				Mutable->SaveConfig();
			}
		}
	}
	return FPaths::ConvertRelativePathToFull(Path);
}

bool FGodotExportPipeline::IsSupportedClass(const UClass* Class, const UGodotExportSettings* Settings)
{
	const UGodotExportSettings* Resolved = Settings ? Settings : GetDefault<UGodotExportSettings>();
	if (!Class || !Resolved)
	{
		return false;
	}

	if (Class->IsChildOf(UStaticMesh::StaticClass()))
	{
		return Resolved->bExportStaticMeshes;
	}
	if (Class->IsChildOf(USkeletalMesh::StaticClass()))
	{
		return Resolved->bExportSkeletalMeshes;
	}
	if (Class->IsChildOf(UAnimSequence::StaticClass()))
	{
		return Resolved->bExportAnimations;
	}
	if (Class->IsChildOf(UTexture2D::StaticClass()))
	{
		return Resolved->bExportTextures;
	}
	if (Class->IsChildOf(UMaterialInterface::StaticClass()))
	{
		return Resolved->bExportMaterials;
	}
	if (Class->IsChildOf(USoundWave::StaticClass()))
	{
		return Resolved->bExportAudio;
	}
	if (Class->IsChildOf(UWorld::StaticClass()))
	{
		return Resolved->bExportLevels;
	}
	if (Class->IsChildOf(UDataTable::StaticClass()))
	{
		return Resolved->bExportDataTables;
	}
	return false;
}

FGodotExportResult FGodotExportPipeline::ExportAll(const UGodotExportSettings* Settings)
{
	const UGodotExportSettings* Resolved = Settings ? Settings : GetDefault<UGodotExportSettings>();
	return ExportContentPath(Resolved->SourceContentPath, Resolved, true);
}

FGodotExportResult FGodotExportPipeline::ExportContentPath(
	const FString& UnrealContentPath,
	const UGodotExportSettings* Settings,
	bool bPromptForProjectIfMissing)
{
	const UGodotExportSettings* Resolved = Settings ? Settings : GetDefault<UGodotExportSettings>();
	const FString GodotProject = ResolveGodotProjectPath(Resolved, bPromptForProjectIfMissing);
	if (GodotProject.IsEmpty())
	{
		FGodotExportResult Empty;
		return Empty;
	}

	FString Path = GodotExportPrivate::ContentBrowserFolderToPackagePath(UnrealContentPath);
	if (Path.IsEmpty())
	{
		Path = TEXT("/Game");
	}

	IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	TArray<FAssetData> Assets;
	Registry.GetAssetsByPath(FName(*Path), Assets, true, false);

	FGodotExportPipeline Pipeline(Resolved, GodotProject);
	return Pipeline.Run(MoveTemp(Assets), false);
}

FGodotExportResult FGodotExportPipeline::ExportAssets(
	const TArray<FAssetData>& InAssets,
	const UGodotExportSettings* Settings,
	bool bPromptForProjectIfMissing)
{
	const UGodotExportSettings* Resolved = Settings ? Settings : GetDefault<UGodotExportSettings>();
	const FString GodotProject = ResolveGodotProjectPath(Resolved, bPromptForProjectIfMissing);
	if (GodotProject.IsEmpty())
	{
		return FGodotExportResult();
	}

	FGodotExportPipeline Pipeline(Resolved, GodotProject, true);
	return Pipeline.Run(InAssets, Resolved->bIncludeDependencies);
}

FString FGodotExportPipeline::DefaultExtensionForAsset(const FAssetData& AssetData, const UGodotExportSettings* Settings)
{
	if (GodotExportPrivate::IsAnimSequenceAsset(AssetData)
		|| GodotExportPrivate::MatchesClass(AssetData, UStaticMesh::StaticClass())
		|| GodotExportPrivate::MatchesClass(AssetData, USkeletalMesh::StaticClass())
		|| GodotExportPrivate::MatchesClass(AssetData, UWorld::StaticClass()))
	{
		return (Settings && !Settings->bUseBinaryGlb) ? TEXT("gltf") : TEXT("glb");
	}
	if (GodotExportPrivate::MatchesClass(AssetData, UTexture2D::StaticClass()))
	{
		return TEXT("png");
	}
	if (GodotExportPrivate::MatchesClass(AssetData, USoundWave::StaticClass()))
	{
		return TEXT("wav");
	}
	if (GodotExportPrivate::MatchesClass(AssetData, UMaterialInterface::StaticClass()))
	{
		return TEXT("tres");
	}
	if (GodotExportPrivate::MatchesClass(AssetData, UDataTable::StaticClass()))
	{
		return TEXT("json");
	}
	return TEXT("glb");
}

FString FGodotExportPipeline::SaveDialogFilterForAsset(const FAssetData& AssetData, const UGodotExportSettings* Settings)
{
	const FString Ext = DefaultExtensionForAsset(AssetData, Settings);
	if (Ext == TEXT("glb"))
	{
		return TEXT("glTF Binary (*.glb)|*.glb");
	}
	if (Ext == TEXT("gltf"))
	{
		return TEXT("glTF (*.gltf)|*.gltf");
	}
	if (Ext == TEXT("png"))
	{
		return TEXT("PNG Image (*.png)|*.png");
	}
	if (Ext == TEXT("wav"))
	{
		return TEXT("WAV Audio (*.wav)|*.wav");
	}
	if (Ext == TEXT("tres"))
	{
		return TEXT("Godot Material (*.tres)|*.tres");
	}
	if (Ext == TEXT("json"))
	{
		return TEXT("JSON (*.json)|*.json");
	}
	return FString::Printf(TEXT("All Files (*.%s)|*.%s"), *Ext, *Ext);
}

FGodotExportResult FGodotExportPipeline::ExportAssetsStandalone(
	const TArray<FAssetData>& InAssets,
	const UGodotExportSettings* Settings)
{
	const UGodotExportSettings* Resolved = Settings ? Settings : GetDefault<UGodotExportSettings>();

	TArray<FAssetData> Exportable;
	for (const FAssetData& Asset : InAssets)
	{
		if (Asset.IsValid() && !Asset.IsRedirector() && GodotExportPrivate::IsSupportedClassForFilter(Asset))
		{
			Exportable.Add(Asset);
		}
	}

	if (Exportable.Num() == 0)
	{
		return FGodotExportResult();
	}

	FString Folder;
	if (!PromptForDirectory(TEXT("Select destination (an 'export' folder will be created)"), Folder))
	{
		FGodotExportResult Cancelled;
		FGodotExportItemResult Item;
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Cancelled");
		Cancelled.Items.Add(Item);
		Cancelled.NumSkipped = 1;
		return Cancelled;
	}

	FGodotExportPipeline Pipeline(Resolved, Folder, false);
	return Pipeline.Run(Exportable, false);
}

FGodotExportResult FGodotExportPipeline::ExportContentPathStandalone(
	const FString& UnrealContentPath,
	const UGodotExportSettings* Settings)
{
	const UGodotExportSettings* Resolved = Settings ? Settings : GetDefault<UGodotExportSettings>();
	FString Folder;
	if (!PromptForDirectory(TEXT("Select destination (an 'export' folder will be created)"), Folder))
	{
		return FGodotExportResult();
	}

	FString Path = GodotExportPrivate::ContentBrowserFolderToPackagePath(UnrealContentPath);
	if (Path.IsEmpty())
	{
		Path = TEXT("/Game");
	}

	IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	TArray<FAssetData> Assets;
	Registry.GetAssetsByPath(FName(*Path), Assets, true, false);

	FGodotExportPipeline Pipeline(Resolved, Folder, false);
	return Pipeline.Run(MoveTemp(Assets), false);
}

FGodotExportResult FGodotExportPipeline::ExportStandaloneSingle(const FAssetData& AssetData)
{
	FGodotExportResult Result;
	FGodotExportItemResult Item;
	Item.AssetPath = AssetData.GetObjectPathString();

	const FString Extension = DefaultExtensionForAsset(AssetData, Settings);
	const FString DefaultName = GodotExportPrivate::SanitizePathSegment(AssetData.AssetName.ToString(), true) + TEXT(".") + Extension;

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Desktop platform unavailable");
		Result.Items.Add(Item);
		Result.NumFailed = 1;
		return Result;
	}

	FString DefaultPath = FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_EXPORT);
	if (DefaultPath.IsEmpty())
	{
		DefaultPath = FPaths::ProjectSavedDir();
	}

	TArray<FString> Files;
	const void* ParentWindow = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(TSharedPtr<SWidget>());
	const bool bPicked = DesktopPlatform->SaveFileDialog(
		ParentWindow,
		TEXT("Export to Godot"),
		DefaultPath,
		DefaultName,
		SaveDialogFilterForAsset(AssetData, Settings),
		0,
		Files);

	if (!bPicked || Files.Num() == 0)
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Cancelled");
		Result.Items.Add(Item);
		Result.NumSkipped = 1;
		return Result;
	}

	OutputPathOverride = Files[0];
	if (FPaths::GetExtension(OutputPathOverride, false).IsEmpty())
	{
		OutputPathOverride += TEXT(".") + Extension;
	}
	GodotProject = FPaths::GetPath(OutputPathOverride);
	bFlattenSidecars = true;
	FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_EXPORT, GodotProject);

	Item = ExportOne(AssetData);
	OutputPathOverride.Empty();
	Result.Items.Add(Item);
	switch (Item.Status)
	{
	case EGodotExportStatus::Succeeded: Result.NumSucceeded = 1; break;
	case EGodotExportStatus::Failed: Result.NumFailed = 1; break;
	case EGodotExportStatus::Skipped: Result.NumSkipped = 1; break;
	default: Result.NumUnsupported = 1; break;
	}
	return Result;
}

FString FGodotExportPipeline::ResolveOutputPath(const FAssetData& AssetData, const FString& Extension) const
{
	if (!OutputPathOverride.IsEmpty())
	{
		return OutputPathOverride;
	}
	return PackageToAbsolutePath(GodotProject, AssetData.PackageName.ToString(), Extension, Settings, AssetData);
}

bool FGodotExportPipeline::IsSingleAnimSequenceSelection(const TArray<FAssetData>& Assets, FAssetData& OutAnim)
{
	int32 AnimCount = 0;
	for (const FAssetData& Asset : Assets)
	{
		if (!Asset.IsValid() || Asset.IsRedirector())
		{
			continue;
		}
		if (GodotExportPrivate::IsAnimSequenceAsset(Asset))
		{
			OutAnim = Asset;
			++AnimCount;
			continue;
		}
		if (GodotExportPrivate::IsSupportedClassForFilter(Asset))
		{
			return false;
		}
	}
	return AnimCount == 1;
}

void FGodotExportPipeline::ExpandDependencies(TArray<FAssetData>& Assets) const
{
	IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get();
	TSet<FName> Seen;
	TArray<FAssetData> Queue = Assets;

	for (const FAssetData& Asset : Assets)
	{
		Seen.Add(Asset.PackageName);
	}

	for (int32 Index = 0; Index < Queue.Num(); ++Index)
	{
		if (!GodotExportPrivate::ShouldPullDependencies(Queue[Index]))
		{
			continue;
		}

		TArray<FName> Dependencies;
		Registry.GetDependencies(Queue[Index].PackageName, Dependencies);
		for (const FName& Dep : Dependencies)
		{
			const FString DepString = Dep.ToString();
			if (Seen.Contains(Dep) || GodotExportPrivate::IsEngineOrScriptPackage(DepString))
			{
				continue;
			}
			Seen.Add(Dep);

			TArray<FAssetData> DepAssets;
			Registry.GetAssetsByPackageName(Dep, DepAssets);
			for (const FAssetData& DepAsset : DepAssets)
			{
				Queue.Add(DepAsset);
				Assets.Add(DepAsset);
			}
		}
	}
}

void FGodotExportPipeline::SortForExport(TArray<FAssetData>& Assets) const
{
	Assets.Sort([](const FAssetData& A, const FAssetData& B)
	{
		const int32 PA = GodotExportPrivate::TypePriority(A.GetClass());
		const int32 PB = GodotExportPrivate::TypePriority(B.GetClass());
		if (PA != PB)
		{
			return PA < PB;
		}
		return A.PackageName.ToString() < B.PackageName.ToString();
	});
}

FGodotExportResult FGodotExportPipeline::Run(TArray<FAssetData> Assets, bool bExpandDependencies)
{
	FGodotExportResult Result;
	if (!Settings || GodotProject.IsEmpty())
	{
		return Result;
	}

	IFileManager::Get().MakeDirectory(*GodotProject, true);

	const bool bAnimSequenceOnly = GodotExportPrivate::SelectionIsAnimSequenceOnly(Assets);
	if (bGodotProjectMode && !bAnimSequenceOnly)
	{
		WriteProjectGodotIfNeeded();
	}

	if (bExpandDependencies && !bAnimSequenceOnly)
	{
		ExpandDependencies(Assets);
	}

	TArray<FAssetData> Unique;
	TSet<FString> SeenPaths;
	for (const FAssetData& Asset : Assets)
	{
		if (!Asset.IsValid() || Asset.IsRedirector())
		{
			continue;
		}
		if (bAnimSequenceOnly && !GodotExportPrivate::IsAnimSequenceAsset(Asset))
		{
			continue;
		}

		const FString PackageName = Asset.PackageName.ToString();
		if (GodotExportPrivate::IsEngineOrScriptPackage(PackageName))
		{
			continue;
		}
		if (Settings->bSkipDeveloperFolders && PackageName.StartsWith(TEXT("/Game/Developers")))
		{
			continue;
		}

		const FString ObjectPath = Asset.GetObjectPathString();
		if (SeenPaths.Contains(ObjectPath))
		{
			continue;
		}

		SeenPaths.Add(ObjectPath);
		Unique.Add(Asset);
	}

	SortForExport(Unique);

	FScopedSlowTask SlowTask(static_cast<float>(Unique.Num()), NSLOCTEXT("GodotExporter", "Exporting", "Exporting assets to Godot..."));
	SlowTask.MakeDialog(true);

	for (const FAssetData& Asset : Unique)
	{
		if (SlowTask.ShouldCancel())
		{
			break;
		}

		SlowTask.EnterProgressFrame(1.f, FText::FromName(Asset.AssetName));
		FGodotExportItemResult Item = ExportOne(Asset);
		switch (Item.Status)
		{
		case EGodotExportStatus::Succeeded: Result.NumSucceeded++; break;
		case EGodotExportStatus::Failed: Result.NumFailed++; break;
		case EGodotExportStatus::Skipped: Result.NumSkipped++; break;
		case EGodotExportStatus::Unsupported: Result.NumUnsupported++; break;
		default: break;
		}
		Result.Items.Add(MoveTemp(Item));
	}

	if (bGodotProjectMode && Settings->bWriteManifest && !bAnimSequenceOnly)
	{
		WriteManifest(Result);
	}

	UE_LOG(LogGodotExporter, Display, TEXT("%s"), *Result.ToSummary());
	return Result;
}

bool FGodotExportPipeline::ShouldSkipUnchanged(const FAssetData& AssetData, const FString& DestinationFile) const
{
	if (!Settings->bSkipUnchanged || !FPaths::FileExists(DestinationFile))
	{
		return false;
	}

	const FString PackageFilename = FPackageName::LongPackageNameToFilename(
		AssetData.PackageName.ToString(),
		FPackageName::GetAssetPackageExtension());

	if (!FPaths::FileExists(PackageFilename))
	{
		return false;
	}

	const FDateTime SourceTime = IFileManager::Get().GetTimeStamp(*PackageFilename);
	const FDateTime DestTime = IFileManager::Get().GetTimeStamp(*DestinationFile);
	return DestTime >= SourceTime && SourceTime.GetTicks() > 0;
}

FGodotExportItemResult FGodotExportPipeline::ExportOne(const FAssetData& AssetData)
{
	FGodotExportItemResult Item;
	Item.AssetPath = AssetData.GetObjectPathString();

	const UClass* Class = AssetData.GetClass();
	if (!IsSupportedClass(Class, Settings))
	{
		Item.Status = EGodotExportStatus::Unsupported;
		Item.Message = TEXT("Unsupported or disabled asset type");
		return Item;
	}

	UObject* Asset = AssetData.GetAsset();
	if (!Asset)
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Failed to load asset");
		return Item;
	}

	if (UTexture2D* Texture = Cast<UTexture2D>(Asset))
	{
		ExportTexture(Texture, AssetData, Item);
		return Item;
	}
	if (USoundWave* Sound = Cast<USoundWave>(Asset))
	{
		ExportAudio(Sound, AssetData, Item);
		return Item;
	}
	if (UMaterialInterface* Material = Cast<UMaterialInterface>(Asset))
	{
		ExportMaterial(Material, AssetData, Item);
		return Item;
	}
	if (UDataTable* Table = Cast<UDataTable>(Asset))
	{
		ExportDataTable(Table, AssetData, Item);
		return Item;
	}
	if (UWorld* World = Cast<UWorld>(Asset))
	{
		ExportLevel(World, AssetData, Item);
		return Item;
	}
	if (Cast<UStaticMesh>(Asset) || Cast<USkeletalMesh>(Asset) || Cast<UAnimSequence>(Asset))
	{
		ExportMeshOrAnim(Asset, AssetData, Item);
		return Item;
	}

	Item.Status = EGodotExportStatus::Unsupported;
	Item.Message = TEXT("No exporter for this class");
	return Item;
}

bool FGodotExportPipeline::ExportWithGltf(UObject* Object, const FString& AbsolutePath, FString& OutMessage)
{
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(AbsolutePath), true);

	UGLTFExportOptions* Options = NewObject<UGLTFExportOptions>();
	Options->ResetToDefault();
	Options->ExportUniformScale = Settings->UniformScale;
	Options->bAdjustNormalmaps = Settings->bFlipNormalMapGreenChannel;
	Options->bExportVertexSkinWeights = true;
	Options->bExportAnimationSequences = true;
	Options->bExportMorphTargets = true;
	Options->bExportVertexColors = false;
	Options->bExportCameras = true;
	Options->bExportLights = true;
	Options->bExportSourceModel = true;

	const bool bAnimationOnly = Object->IsA<UAnimSequence>();
	Options->bExportPreviewMesh = false;
	if (bAnimationOnly)
	{
		Options->bExportCameras = false;
		Options->bExportLights = false;
		Options->BakeMaterialInputs = EGLTFMaterialBakeMode::Disabled;
		Options->TextureImageFormat = EGLTFTextureImageFormat::None;
	}
	else
	{
		Options->bExportUnlitMaterials = true;
		Options->bExportClearCoatMaterials = true;
		Options->BakeMaterialInputs = EGLTFMaterialBakeMode::Simple;
		Options->TextureImageFormat = EGLTFTextureImageFormat::PNG;
	}

	FGLTFExportMessages Messages;
	const TSet<AActor*> SelectedActors;
	const bool bOk = UGLTFExporter::ExportToGLTF(Object, AbsolutePath, Options, SelectedActors, Messages);

	TArray<FString> Combined;
	Combined.Append(Messages.Errors);
	Combined.Append(Messages.Warnings);
	if (Combined.Num() > 0)
	{
		OutMessage = FString::Join(Combined, TEXT("\n"));
	}
	else
	{
		OutMessage = bOk ? TEXT("Exported glTF") : TEXT("glTF export failed");
	}
	return bOk;
}

void FGodotExportPipeline::CleanupGltfImagesInMeshFolder(const FString& GltfAbsolutePath)
{
	const FString MeshDir = FPaths::GetPath(GltfAbsolutePath);
	const bool bIsGlb = FPaths::GetExtension(GltfAbsolutePath, false).Equals(TEXT("glb"), ESearchCase::IgnoreCase);

	FString Subfolder = (Settings && !Settings->ExportSubfolder.IsEmpty()) ? Settings->ExportSubfolder : TEXT("export");
	const FString TexturesDir = FPaths::Combine(GodotProject, Subfolder, TEXT("textures"));

	TArray<FString> Found;
	IFileManager::Get().FindFiles(Found, *FPaths::Combine(MeshDir, TEXT("*.*")), true, false);

	TArray<FString> MovedNames;
	for (const FString& Name : Found)
	{
		const FString Ext = FPaths::GetExtension(Name, false).ToLower();
		if (Ext != TEXT("png") && Ext != TEXT("jpg") && Ext != TEXT("jpeg") && Ext != TEXT("tga") && Ext != TEXT("webp"))
		{
			continue;
		}

		const FString SourceFile = FPaths::Combine(MeshDir, Name);
		if (bIsGlb)
		{
			IFileManager::Get().Delete(*SourceFile);
			continue;
		}

		IFileManager::Get().MakeDirectory(*TexturesDir, true);
		const FString DestFile = FPaths::Combine(TexturesDir, Name);
		IFileManager::Get().Move(*DestFile, *SourceFile, true);
		MovedNames.Add(Name);
	}

	if (!bIsGlb && MovedNames.Num() > 0 && FPaths::FileExists(GltfAbsolutePath))
	{
		FString Json;
		if (FFileHelper::LoadFileToString(Json, *GltfAbsolutePath))
		{
			for (const FString& Name : MovedNames)
			{
				Json.ReplaceInline(*Name, *(FString(TEXT("../textures/")) + Name), ESearchCase::CaseSensitive);
			}
			FFileHelper::SaveStringToFile(Json, *GltfAbsolutePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
		}
	}
}

bool FGodotExportPipeline::WriteProjectGodotIfNeeded() const
{
	if (!bGodotProjectMode || !Settings->bWriteProjectGodotIfMissing)
	{
		return true;
	}

	const FString ProjectFile = FPaths::Combine(GodotProject, TEXT("project.godot"));
	if (FPaths::FileExists(ProjectFile))
	{
		return true;
	}

	const FString Contents =
		TEXT("; Engine configuration file.\n")
		TEXT("; Generated by the Unreal GodotExporter plugin.\n")
		TEXT("config_version=5\n\n")
		TEXT("[application]\n\n")
		TEXT("config/name=\"Downgrader Export\"\n")
		TEXT("config/features=PackedStringArray(\"4.3\", \"Forward Plus\")\n\n")
		TEXT("[rendering]\n\n")
		TEXT("renderer/rendering_method=\"forward_plus\"\n");

	return FFileHelper::SaveStringToFile(Contents, *ProjectFile, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

void FGodotExportPipeline::WriteManifest(const FGodotExportResult& Result) const
{
	TStringBuilder<4096> Json;
	Json.Append(TEXT("{\n"));
	Json.Appendf(TEXT("  \"succeeded\": %d,\n"), Result.NumSucceeded);
	Json.Appendf(TEXT("  \"failed\": %d,\n"), Result.NumFailed);
	Json.Appendf(TEXT("  \"skipped\": %d,\n"), Result.NumSkipped);
	Json.Appendf(TEXT("  \"unsupported\": %d,\n"), Result.NumUnsupported);
	Json.Append(TEXT("  \"items\": [\n"));

	for (int32 i = 0; i < Result.Items.Num(); ++i)
	{
		const FGodotExportItemResult& Item = Result.Items[i];
		const FString Status = StaticEnum<EGodotExportStatus>()->GetNameStringByValue(static_cast<int64>(Item.Status));
		Json.Append(TEXT("    {\n"));
		Json.Appendf(TEXT("      \"asset\": \"%s\",\n"), *GodotExportPrivate::EscapeJson(Item.AssetPath));
		Json.Appendf(TEXT("      \"output\": \"%s\",\n"), *GodotExportPrivate::EscapeJson(Item.OutputPath));
		Json.Appendf(TEXT("      \"status\": \"%s\",\n"), *Status);
		Json.Appendf(TEXT("      \"message\": \"%s\"\n"), *GodotExportPrivate::EscapeJson(Item.Message));
		Json.Append(i + 1 < Result.Items.Num() ? TEXT("    },\n") : TEXT("    }\n"));
	}

	Json.Append(TEXT("  ]\n}\n"));

	FString ManifestFolder = GodotProject;
	if (Settings && !Settings->ExportSubfolder.IsEmpty())
	{
		ManifestFolder = FPaths::Combine(GodotProject, Settings->ExportSubfolder);
	}
	const FString ManifestPath = FPaths::Combine(ManifestFolder, TEXT("godot_export_manifest.json"));
	FFileHelper::SaveStringToFile(Json.ToString(), *ManifestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

bool FGodotExportPipeline::ExportMeshOrAnim(UObject* Object, const FAssetData& AssetData, FGodotExportItemResult& Item)
{
	const FString Extension = Settings->bUseBinaryGlb ? TEXT("glb") : TEXT("gltf");
	const FString AbsolutePath = ResolveOutputPath(AssetData, Extension);
	Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);

	if (ShouldSkipUnchanged(AssetData, AbsolutePath))
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Destination is up to date");
		ExportedResPaths.Add(AssetData.PackageName, Item.OutputPath);
		if (!Object->IsA<UAnimSequence>())
		{
			ExportMeshSidecars(Object);
		}
		return true;
	}

	FString Message;
	if (!ExportWithGltf(Object, AbsolutePath, Message))
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = Message;
		return false;
	}

	CleanupGltfImagesInMeshFolder(AbsolutePath);

	if (!Object->IsA<UAnimSequence>())
	{
		const FString SavedOverride = OutputPathOverride;
		OutputPathOverride.Empty();
		ExportMeshSidecars(Object);
		OutputPathOverride = SavedOverride;
	}
	ExportedResPaths.Add(AssetData.PackageName, Item.OutputPath);
	Item.Status = EGodotExportStatus::Succeeded;
	if (Object->IsA<UAnimSequence>())
	{
		Item.Message = TEXT("Exported animation (no mesh, textures or materials)");
	}
	else
	{
		Item.Message = TEXT("Exported mesh .glb plus .tres materials and .png textures");
	}
	return true;
}

void FGodotExportPipeline::CollectMeshMaterials(UObject* MeshObject, TArray<UMaterialInterface*>& OutMaterials) const
{
	if (USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(MeshObject))
	{
		for (const FSkeletalMaterial& Slot : SkeletalMesh->GetMaterials())
		{
			if (Slot.MaterialInterface)
			{
				OutMaterials.AddUnique(Slot.MaterialInterface);
			}
		}
	}
	else if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(MeshObject))
	{
		for (const FStaticMaterial& Slot : StaticMesh->GetStaticMaterials())
		{
			if (Slot.MaterialInterface)
			{
				OutMaterials.AddUnique(Slot.MaterialInterface);
			}
		}
	}
}

void FGodotExportPipeline::ExportMeshSidecars(UObject* MeshObject)
{
	if (!Settings->bGenerateGodotMaterials && !Settings->bExportTextures)
	{
		return;
	}

	TArray<UMaterialInterface*> Materials;
	CollectMeshMaterials(MeshObject, Materials);
	for (UMaterialInterface* Material : Materials)
	{
		if (!Material)
		{
			continue;
		}
		FGodotExportItemResult MatItem;
		ExportMaterial(Material, FAssetData(Material), MatItem);
	}
}

bool FGodotExportPipeline::CopyOriginalSourceFile(UObject* Asset, const FString& DestinationFile, FString& OutUsedExtension) const
{
	if (!Settings->bPreferOriginalSourceFiles)
	{
		return false;
	}

	UAssetImportData* ImportData = GodotExportPrivate::FindImportData(Asset);
	if (!ImportData)
	{
		return false;
	}

	const FString SourceFile = ImportData->GetFirstFilename();
	if (SourceFile.IsEmpty() || !FPaths::FileExists(SourceFile))
	{
		return false;
	}

	OutUsedExtension = FPaths::GetExtension(SourceFile, false).ToLower();
	const TArray<FString> Allowed = { TEXT("png"), TEXT("jpg"), TEXT("jpeg"), TEXT("tga"), TEXT("webp"), TEXT("exr"), TEXT("wav"), TEXT("ogg"), TEXT("mp3") };
	if (!Allowed.Contains(OutUsedExtension))
	{
		return false;
	}

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(DestinationFile), true);
	return IFileManager::Get().Copy(*DestinationFile, *SourceFile, true, true) == COPY_OK;
}

bool FGodotExportPipeline::ExportTexture(UTexture2D* Texture, const FAssetData& AssetData, FGodotExportItemResult& Item)
{
	FString AbsolutePath = ResolveOutputPath(AssetData, TEXT("png"));

	const FString SourceFile = Texture->AssetImportData ? Texture->AssetImportData->GetFirstFilename() : FString();
	if (Settings->bPreferOriginalSourceFiles && FPaths::FileExists(SourceFile))
	{
		const FString UsedExt = FPaths::GetExtension(SourceFile, false).ToLower();
		AbsolutePath = ResolveOutputPath(AssetData, UsedExt);
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(AbsolutePath), true);
		if (IFileManager::Get().Copy(*AbsolutePath, *SourceFile, true, true) == COPY_OK)
		{
			Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);
			ExportedResPaths.Add(AssetData.PackageName, Item.OutputPath);
			Item.Status = EGodotExportStatus::Succeeded;
			Item.Message = TEXT("Copied original source texture");
			return true;
		}
	}

	FImage Image;
	if (!FImageUtils::GetTexture2DSourceImage(Texture, Image))
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Could not read texture source (unsupported or missing source data)");
		return false;
	}

	const bool bHDR = ERawImageFormat::IsHDR(Image.Format);
	const bool bFlipGreen = Settings->bFlipNormalMapGreenChannel && Texture->CompressionSettings == TC_Normalmap;
	const FString Extension = (!bFlipGreen && bHDR) ? TEXT("exr") : TEXT("png");
	AbsolutePath = ResolveOutputPath(AssetData, Extension);
	Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);

	if (ShouldSkipUnchanged(AssetData, AbsolutePath))
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Destination is up to date");
		ExportedResPaths.Add(AssetData.PackageName, Item.OutputPath);
		return true;
	}

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(AbsolutePath), true);

	bool bSaved = false;
	if (bFlipGreen)
	{
		FImage Bgra;
		Image.CopyTo(Bgra, ERawImageFormat::BGRA8, EGammaSpace::Linear);
		uint8* Data = Bgra.RawData.GetData();
		const int64 Count = Bgra.RawData.Num();
		for (int64 i = 1; i < Count; i += 4)
		{
			Data[i] = static_cast<uint8>(255 - Data[i]);
		}
		bSaved = FImageUtils::SaveImageByExtension(*AbsolutePath, Bgra);
	}
	else
	{
		bSaved = FImageUtils::SaveImageByExtension(*AbsolutePath, Image);
	}

	if (!bSaved)
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Failed to write texture file");
		return false;
	}

	ExportedResPaths.Add(AssetData.PackageName, Item.OutputPath);
	Item.Status = EGodotExportStatus::Succeeded;
	if (bFlipGreen)
	{
		Item.Message = TEXT("Exported PNG (normal map green channel flipped)");
	}
	else if (bHDR)
	{
		Item.Message = TEXT("Exported EXR (HDR source)");
	}
	else
	{
		Item.Message = TEXT("Exported PNG");
	}
	return true;
}

bool FGodotExportPipeline::ExportAudio(USoundWave* SoundWave, const FAssetData& AssetData, FGodotExportItemResult& Item)
{
	FString AbsolutePath = ResolveOutputPath(AssetData, TEXT("wav"));

	const FString SourceFile = SoundWave->AssetImportData ? SoundWave->AssetImportData->GetFirstFilename() : FString();
	if (Settings->bPreferOriginalSourceFiles && FPaths::FileExists(SourceFile))
	{
		const FString Ext = FPaths::GetExtension(SourceFile, false).ToLower();
		AbsolutePath = ResolveOutputPath(AssetData, Ext);
		IFileManager::Get().MakeDirectory(*FPaths::GetPath(AbsolutePath), true);
		if (IFileManager::Get().Copy(*AbsolutePath, *SourceFile, true, true) == COPY_OK)
		{
			Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);
			Item.Status = EGodotExportStatus::Succeeded;
			Item.Message = TEXT("Copied original audio file");
			return true;
		}
		AbsolutePath = ResolveOutputPath(AssetData, TEXT("wav"));
	}

	Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);
	if (ShouldSkipUnchanged(AssetData, AbsolutePath))
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Destination is up to date");
		return true;
	}

	TArray<uint8> PCM;
	uint32 SampleRate = 0;
	uint16 NumChannels = 0;
	if (!SoundWave->GetImportedSoundWaveData(PCM, SampleRate, NumChannels) || PCM.Num() == 0 || SampleRate == 0)
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Could not read PCM data from SoundWave");
		return false;
	}

	if (!GodotExportPrivate::WriteWavFile(AbsolutePath, PCM, SampleRate, NumChannels, 16))
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Failed to write WAV file");
		return false;
	}

	Item.Status = EGodotExportStatus::Succeeded;
	Item.Message = TEXT("Exported WAV");
	return true;
}

bool FGodotExportPipeline::ExportMaterial(UMaterialInterface* Material, const FAssetData& AssetData, FGodotExportItemResult& Item)
{
	if (!Settings->bGenerateGodotMaterials)
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Godot material generation disabled");
		return true;
	}

	const FString AbsolutePath = ResolveOutputPath(AssetData, TEXT("tres"));
	Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);
	if (ShouldSkipUnchanged(AssetData, AbsolutePath))
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Destination is up to date");
		return true;
	}

	TMap<FString, FString> RoleToRes;
	TMap<FString, FLinearColor> VectorParams;
	TMap<FString, float> ScalarParams;

	auto RememberTexture = [this, &RoleToRes](const FString& HintName, UTexture* Texture)
	{
		if (!Texture || GodotExportPrivate::IsEngineTexture(Texture))
		{
			return;
		}

		FString Role = GodotExportPrivate::GuessTextureRole(HintName);
		if (Role.IsEmpty())
		{
			Role = GodotExportPrivate::GuessTextureRole(Texture->GetName());
		}
		if (Role.IsEmpty() || RoleToRes.Contains(Role))
		{
			return;
		}

		FString ResPath;
		if (const FString* Existing = ExportedResPaths.Find(Texture->GetOutermost()->GetFName()))
		{
			ResPath = *Existing;
		}
		else if (UTexture2D* Tex2D = Cast<UTexture2D>(Texture))
		{
			FGodotExportItemResult Nested;
			const FAssetData NestedData(Tex2D);
			if (ExportTexture(Tex2D, NestedData, Nested)
				&& (Nested.Status == EGodotExportStatus::Succeeded || Nested.Status == EGodotExportStatus::Skipped))
			{
				ResPath = Nested.OutputPath;
			}
		}
		if (!ResPath.IsEmpty())
		{
			RoleToRes.Add(Role, ResPath);
		}
	};

	if (UMaterialInstance* Instance = Cast<UMaterialInstance>(Material))
	{
		for (const FTextureParameterValue& Param : Instance->TextureParameterValues)
		{
			RememberTexture(Param.ParameterInfo.Name.ToString(), Param.ParameterValue);
		}
		for (const FVectorParameterValue& Param : Instance->VectorParameterValues)
		{
			VectorParams.Add(Param.ParameterInfo.Name.ToString(), Param.ParameterValue);
		}
		for (const FScalarParameterValue& Param : Instance->ScalarParameterValues)
		{
			ScalarParams.Add(Param.ParameterInfo.Name.ToString(), Param.ParameterValue);
		}
	}
	else
	{
		TArray<FMaterialParameterInfo> TextureInfos;
		TArray<FGuid> TextureIds;
		Material->GetAllTextureParameterInfo(TextureInfos, TextureIds);
		for (const FMaterialParameterInfo& Info : TextureInfos)
		{
			UTexture* Texture = nullptr;
			Material->GetTextureParameterValue(Info, Texture);
			RememberTexture(Info.Name.ToString(), Texture);
		}
	}

	FLinearColor Albedo = FLinearColor::White;
	for (const TPair<FString, FLinearColor>& Pair : VectorParams)
	{
		if (GodotExportPrivate::GuessTextureRole(Pair.Key) == TEXT("albedo"))
		{
			Albedo = Pair.Value;
			break;
		}
	}

	float Metallic = 0.f;
	float Roughness = 1.f;
	for (const TPair<FString, float>& Pair : ScalarParams)
	{
		const FString Role = GodotExportPrivate::GuessTextureRole(Pair.Key);
		if (Role == TEXT("metallic"))
		{
			Metallic = Pair.Value;
		}
		else if (Role == TEXT("roughness"))
		{
			Roughness = Pair.Value;
		}
	}

	const bool bTwoSided = Material->IsTwoSided();
	const EBlendMode Blend = Material->GetBlendMode();
	int32 Transparency = 0;
	if (Blend == BLEND_Masked)
	{
		Transparency = 2;
	}
	else if (Blend == BLEND_Translucent || Blend == BLEND_Additive || Blend == BLEND_Modulate)
	{
		Transparency = 1;
	}

	TArray<FString> ExtResources;
	TArray<FString> ResourceLines;
	int32 ResourceId = 1;
	TMap<FString, int32> RoleToId;
	for (const TPair<FString, FString>& Pair : RoleToRes)
	{
		RoleToId.Add(Pair.Key, ResourceId);
		ExtResources.Add(FString::Printf(
			TEXT("[ext_resource type=\"Texture2D\" path=\"%s\" id=\"%d_tex\"]"),
			*Pair.Value,
			ResourceId));
		++ResourceId;
	}

	ResourceLines.Add(FString::Printf(TEXT("albedo_color = Color(%f, %f, %f, %f)"), Albedo.R, Albedo.G, Albedo.B, Albedo.A));
	ResourceLines.Add(FString::Printf(TEXT("metallic = %f"), Metallic));
	ResourceLines.Add(FString::Printf(TEXT("roughness = %f"), Roughness));
	if (bTwoSided)
	{
		ResourceLines.Add(TEXT("cull_mode = 2"));
	}
	if (Transparency != 0)
	{
		ResourceLines.Add(FString::Printf(TEXT("transparency = %d"), Transparency));
	}

	auto BindTexture = [&](const TCHAR* Role, const TCHAR* Property, const TCHAR* Extra = nullptr)
	{
		if (const int32* Id = RoleToId.Find(Role))
		{
			ResourceLines.Add(FString::Printf(TEXT("%s = ExtResource(\"%d_tex\")"), Property, *Id));
			if (Extra)
			{
				ResourceLines.Add(FString(Extra));
			}
		}
	};

	BindTexture(TEXT("albedo"), TEXT("albedo_texture"));
	BindTexture(TEXT("normal"), TEXT("normal_texture"), TEXT("normal_enabled = true"));
	BindTexture(TEXT("emission"), TEXT("emission_texture"), TEXT("emission_enabled = true"));
	BindTexture(TEXT("roughness"), TEXT("roughness_texture"));
	BindTexture(TEXT("metallic"), TEXT("metallic_texture"));
	BindTexture(TEXT("ao"), TEXT("ao_texture"), TEXT("ao_enabled = true"));
	BindTexture(TEXT("alpha"), TEXT("albedo_texture"));

	if (const int32* OrmId = RoleToId.Find(TEXT("orm")))
	{
		ResourceLines.Add(FString::Printf(TEXT("ao_texture = ExtResource(\"%d_tex\")"), *OrmId));
		ResourceLines.Add(TEXT("ao_enabled = true"));
		ResourceLines.Add(TEXT("ao_texture_channel = 0"));
		ResourceLines.Add(FString::Printf(TEXT("roughness_texture = ExtResource(\"%d_tex\")"), *OrmId));
		ResourceLines.Add(TEXT("roughness_texture_channel = 1"));
		ResourceLines.Add(FString::Printf(TEXT("metallic_texture = ExtResource(\"%d_tex\")"), *OrmId));
		ResourceLines.Add(TEXT("metallic_texture_channel = 2"));
	}

	FString Contents = FString::Printf(TEXT("[gd_resource type=\"StandardMaterial3D\" format=3]\n\n"));
	for (const FString& Line : ExtResources)
	{
		Contents += Line + TEXT("\n");
	}
	if (ExtResources.Num() > 0)
	{
		Contents += TEXT("\n");
	}
	Contents += TEXT("[resource]\n");
	for (const FString& Line : ResourceLines)
	{
		Contents += Line + TEXT("\n");
	}

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(AbsolutePath), true);
	if (!FFileHelper::SaveStringToFile(Contents, *AbsolutePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Failed to write .tres material");
		return false;
	}

	Item.Status = EGodotExportStatus::Succeeded;
	Item.Message = TEXT("Wrote StandardMaterial3D");
	return true;
}

bool FGodotExportPipeline::ExportLevel(UWorld* World, const FAssetData& AssetData, FGodotExportItemResult& Item)
{
	const FString Extension = Settings->bUseBinaryGlb ? TEXT("glb") : TEXT("gltf");
	const FString AbsolutePath = ResolveOutputPath(AssetData, Extension);
	Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);

	if (ShouldSkipUnchanged(AssetData, AbsolutePath))
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Destination is up to date");
		return true;
	}

	FString Message;
	if (!ExportWithGltf(World, AbsolutePath, Message))
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = Message;
		return false;
	}

	CleanupGltfImagesInMeshFolder(AbsolutePath);

	Item.Status = EGodotExportStatus::Succeeded;
	Item.Message = Message;
	return true;
}

bool FGodotExportPipeline::ExportDataTable(UDataTable* DataTable, const FAssetData& AssetData, FGodotExportItemResult& Item)
{
	const FString AbsolutePath = ResolveOutputPath(AssetData, TEXT("json"));
	Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);

	if (ShouldSkipUnchanged(AssetData, AbsolutePath))
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Destination is up to date");
		return true;
	}

	const FString Json = DataTable->GetTableAsJSON();
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(AbsolutePath), true);
	if (!FFileHelper::SaveStringToFile(Json, *AbsolutePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Failed to write JSON");
		return false;
	}

	Item.Status = EGodotExportStatus::Succeeded;
	Item.Message = TEXT("Exported DataTable as JSON");
	return true;
}
