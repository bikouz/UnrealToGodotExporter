#include "GodotExportPipeline.h"

#include "Animation/AnimSequence.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Components/LightComponent.h"
#include "DesktopPlatformModule.h"
#include "Editor.h"
#include "EditorDirectories.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/DataTable.h"
#include "Engine/Level.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"
#include "Engine/TextureLightProfile.h"
#include "Engine/World.h"
#include "Exporters/GLTFExporter.h"
#include "Framework/Application/SlateApplication.h"
#include "GodotExportSettings.h"
#include "Dom/JsonObject.h"
#include "HAL/FileManager.h"
#include "ImageCore.h"
#include "ImageUtils.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Char.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Options/GLTFExportOptions.h"
#include "Sound/SoundWave.h"
#include "UObject/Package.h"
#include "Widgets/SWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogGodotExporter, Log, All);

namespace GodotExportPrivate
{
	UWorld* ResolveWorldForExport(UWorld* AssetWorld)
	{
		if (!AssetWorld)
		{
			return nullptr;
		}
		if (GEditor)
		{
			if (UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
			{
				if (EditorWorld->GetOutermost() == AssetWorld->GetOutermost())
				{
					return EditorWorld;
				}
			}
		}
		return AssetWorld;
	}

	struct FScopedClearLightIesProfiles
	{
		explicit FScopedClearLightIesProfiles(UWorld* World)
		{
			if (!World)
			{
				return;
			}
			for (ULevel* Level : World->GetLevels())
			{
				if (!Level)
				{
					continue;
				}
				for (AActor* Actor : Level->Actors)
				{
					if (!Actor)
					{
						continue;
					}
					TInlineComponentArray<ULightComponent*> Lights(Actor);
					for (ULightComponent* Light : Lights)
					{
						if (Light && Light->IESTexture)
						{
							Saved.Add({ Light, Light->IESTexture });
							Light->IESTexture = nullptr;
						}
					}
				}
			}
		}

		~FScopedClearLightIesProfiles()
		{
			for (const TPair<ULightComponent*, TObjectPtr<UTextureLightProfile>>& Pair : Saved)
			{
				if (Pair.Key)
				{
					Pair.Key->IESTexture = Pair.Value;
				}
			}
		}

		TArray<TPair<ULightComponent*, TObjectPtr<UTextureLightProfile>>> Saved;
	};
}

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

	FString SanitizeGodotNodeName(const FString& In)
	{
		FString Out = SanitizePathSegment(In, true);
		if (Out.IsEmpty())
		{
			Out = TEXT("Mesh");
		}
		if (FChar::IsDigit(Out[0]))
		{
			Out = TEXT("_") + Out;
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
		if (E == TEXT("tscn"))
		{
			return TEXT("prefabs");
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
		if (MatchesClass(Asset, UWorld::StaticClass()))
		{
			return TEXT("levels");
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

	FString LastNameToken(const FString& Name)
	{
		FString Lower = Name.ToLower();
		Lower.TrimStartAndEndInline();
		int32 Dot = INDEX_NONE;
		if (Lower.FindLastChar(TEXT('.'), Dot) && Dot > 0)
		{
			Lower = Lower.Left(Dot);
		}
		int32 Split = INDEX_NONE;
		if (Lower.FindLastChar(TEXT('_'), Split) && Split + 1 < Lower.Len())
		{
			return Lower.Mid(Split + 1);
		}
		if (Lower.FindLastChar(TEXT(' '), Split) && Split + 1 < Lower.Len())
		{
			return Lower.Mid(Split + 1);
		}
		return Lower;
	}

	FString CompactIdent(const FString& Name)
	{
		FString Out;
		Out.Reserve(Name.Len());
		for (const TCHAR C : Name)
		{
			if (FChar::IsAlnum(C))
			{
				Out.AppendChar(FChar::ToLower(C));
			}
		}
		return Out;
	}

	int32 AlbedoNameScore(const FString& Name)
	{
		const FString Compact = CompactIdent(Name);
		if (Compact == TEXT("basecolor") || Compact == TEXT("basecolour"))
		{
			return 100;
		}
		if (Compact.Contains(TEXT("basecolor")) || Compact.Contains(TEXT("basecolour")))
		{
			return 90;
		}
		if (Compact.Contains(TEXT("albedo")))
		{
			return 80;
		}
		if (Compact.Contains(TEXT("diffuse")))
		{
			return 70;
		}
		if (Compact.Contains(TEXT("basetexture")) || Compact.Contains(TEXT("basemap")))
		{
			return 60;
		}
		if (Compact == TEXT("tint") || Compact == TEXT("tintcolor") || Compact == TEXT("tintcolour"))
		{
			return 50;
		}
		if (Compact == TEXT("color") || Compact == TEXT("colour"))
		{
			return 40;
		}
		return 0;
	}

	bool IsNearlyWhite(const FLinearColor& Color)
	{
		return Color.Equals(FLinearColor::White, 0.02f);
	}

	FLinearColor ResolveAlbedoColor(UMaterialInterface* Material)
	{
		FLinearColor Albedo = FLinearColor::White;
		if (!Material)
		{
			return Albedo;
		}

		// Instance vector overrides win, including white: a MIC that tints to white
		// must not fall back to a coloured parent default.
		{
			UMaterialInterface* Current = Material;
			int32 Depth = 0;
			while (UMaterialInstance* Inst = Cast<UMaterialInstance>(Current))
			{
				int32 BestLocal = 0;
				FLinearColor Local = FLinearColor::White;
				FString LocalName;
				bool bLocal = false;
				for (const FVectorParameterValue& Param : Inst->VectorParameterValues)
				{
					const FString Name = Param.ParameterInfo.Name.ToString();
					const int32 Score = AlbedoNameScore(Name);
					if (Score <= 0)
					{
						continue;
					}
					if (!bLocal || Score > BestLocal)
					{
						BestLocal = Score;
						Local = Param.ParameterValue;
						LocalName = Name;
						bLocal = true;
					}
				}
				if (bLocal)
				{
					UE_LOG(
						LogGodotExporter,
						Display,
						TEXT("Material %s albedo_color=(%f, %f, %f, %f) MIC override '%s' depth=%d"),
						*Material->GetName(),
						Local.R,
						Local.G,
						Local.B,
						Local.A,
						*LocalName,
						Depth);
					return Local;
				}
				Current = Inst->Parent;
				if (++Depth > 16)
				{
					break;
				}
			}
		}

		int32 BestScore = 0;
		bool bFoundNonWhite = false;
		FString ChosenName;

		auto Consider = [&](const FString& Name, const FLinearColor& Value)
		{
			const int32 Score = AlbedoNameScore(Name);
			if (Score <= 0)
			{
				return;
			}
			const bool bNonWhite = !IsNearlyWhite(Value);
			if (bNonWhite && (!bFoundNonWhite || Score > BestScore))
			{
				BestScore = Score;
				Albedo = Value;
				ChosenName = Name;
				bFoundNonWhite = true;
				return;
			}
			if (!bFoundNonWhite && Score > BestScore)
			{
				BestScore = Score;
				Albedo = Value;
				ChosenName = Name;
			}
		};

		static const TCHAR* PreferredNames[] = {
			TEXT("Base Color"),
			TEXT("Base Color "),
			TEXT("BaseColor"),
			TEXT("Color"),
			TEXT("Tint"),
			TEXT("Albedo"),
			TEXT("Diffuse Color"),
			TEXT("DiffuseColor")
		};
		for (const TCHAR* Name : PreferredNames)
		{
			FLinearColor Value;
			if (Material->GetVectorParameterValue(FMaterialParameterInfo(Name), Value))
			{
				Consider(Name, Value);
			}
		}

		TArray<FMaterialParameterInfo> VectorInfos;
		TArray<FGuid> VectorIds;
		Material->GetAllVectorParameterInfo(VectorInfos, VectorIds);
		for (const FMaterialParameterInfo& Info : VectorInfos)
		{
			FLinearColor Value;
			if (Material->GetVectorParameterValue(Info, Value))
			{
				Consider(Info.Name.ToString(), Value);
			}
		}

		UE_LOG(
			LogGodotExporter,
			Display,
			TEXT("Material %s albedo_color=(%f, %f, %f, %f) param='%s'"),
			*Material->GetName(),
			Albedo.R,
			Albedo.G,
			Albedo.B,
			Albedo.A,
			*ChosenName);
		return Albedo;
	}

	FString GuessTextureRole(const FString& Name)
	{
		const FString Token = LastNameToken(Name);
		const FString Lower = Name.ToLower();
		const FString Compact = CompactIdent(Name);

		if (Token == TEXT("n") || Token == TEXT("nrm") || Token == TEXT("norm") || Token == TEXT("normal")
			|| Token == TEXT("normalmap") || Compact.Contains(TEXT("normal")))
		{
			return TEXT("normal");
		}
		if (Token == TEXT("e") || Token == TEXT("em") || Token == TEXT("emissive") || Token == TEXT("emission")
			|| Compact.Contains(TEXT("emissi")))
		{
			return TEXT("emission");
		}
		if (Token == TEXT("orm") || Token == TEXT("arm") || Token == TEXT("rma") || Token == TEXT("mra")
			|| Token == TEXT("mask") || Compact == TEXT("orm") || Compact.EndsWith(TEXT("orm"))
			|| Compact.Contains(TEXT("mra")))
		{
			return TEXT("orm");
		}
		if (Token == TEXT("rough") || Token == TEXT("roughness") || Compact.Contains(TEXT("roughness")))
		{
			return TEXT("roughness");
		}
		if (Token == TEXT("metal") || Token == TEXT("metallic") || Token == TEXT("met") || Compact.Contains(TEXT("metallic")))
		{
			return TEXT("metallic");
		}
		if (Token == TEXT("ao") || Token == TEXT("occlusion") || Compact.Contains(TEXT("ambientocclusion"))
			|| Compact.Contains(TEXT("occlusion")))
		{
			return TEXT("ao");
		}
		if (Token == TEXT("opacity") || Token == TEXT("alpha") || Token == TEXT("alphamask") || Compact.Contains(TEXT("opacity")))
		{
			return TEXT("alpha");
		}
		if (Token == TEXT("d") || Token == TEXT("c") || Token == TEXT("bc") || Token == TEXT("col") || Token == TEXT("diff")
			|| Token == TEXT("diffuse") || Token == TEXT("albedo") || Token == TEXT("basecolor") || Token == TEXT("basemap")
			|| Token == TEXT("color") || Token == TEXT("colour") || Token == TEXT("tint")
			|| AlbedoNameScore(Name) > 0)
		{
			return TEXT("albedo");
		}
		return TEXT("");
	}

	FString RoleFromMaterialProperty(EMaterialProperty Property)
	{
		switch (Property)
		{
		case MP_BaseColor: return TEXT("albedo");
		case MP_Normal: return TEXT("normal");
		case MP_EmissiveColor: return TEXT("emission");
		case MP_Metallic: return TEXT("metallic");
		case MP_Roughness: return TEXT("roughness");
		case MP_AmbientOcclusion: return TEXT("ao");
		case MP_Opacity:
		case MP_OpacityMask: return TEXT("alpha");
		default: return TEXT("");
		}
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
	FString Subfolder = (Settings && !Settings->ExportSubfolder.IsEmpty()) ? Settings->ExportSubfolder : TEXT("assets/export");
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
	if (GodotExportPrivate::MatchesClass(AssetData, UStaticMesh::StaticClass())
		|| GodotExportPrivate::MatchesClass(AssetData, USkeletalMesh::StaticClass())
		|| GodotExportPrivate::IsAnimSequenceAsset(AssetData)
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
	if (!PromptForDirectory(TEXT("Select destination (an 'assets/export' folder will be created)"), Folder))
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
	if (!PromptForDirectory(TEXT("Select destination (an 'assets/export' folder will be created)"), Folder))
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
	Options->bUseImporterMaterialMapping = false;

	const bool bAnimationOnly = Object->IsA<UAnimSequence>();
	const bool bLevel = Object->IsA<UWorld>();
	Options->bExportPreviewMesh = false;
	if (bAnimationOnly)
	{
		Options->bExportCameras = false;
		Options->bExportLights = false;
		Options->BakeMaterialInputs = EGLTFMaterialBakeMode::Disabled;
		Options->TextureImageFormat = EGLTFTextureImageFormat::None;
	}
	else if (bLevel)
	{
		Options->bExportUnlitMaterials = true;
		Options->bExportClearCoatMaterials = true;
		Options->BakeMaterialInputs = EGLTFMaterialBakeMode::Simple;
		Options->TextureImageFormat = EGLTFTextureImageFormat::PNG;
	}
	else
	{
		// Mesh: named material slots, no baked images. URIs to textures/ are patched after export.
		Options->bExportCameras = false;
		Options->bExportLights = false;
		Options->bExportUnlitMaterials = false;
		Options->bExportClearCoatMaterials = false;
		Options->BakeMaterialInputs = EGLTFMaterialBakeMode::Disabled;
		Options->TextureImageFormat = EGLTFTextureImageFormat::None;
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

	FString Subfolder = (Settings && !Settings->ExportSubfolder.IsEmpty()) ? Settings->ExportSubfolder : TEXT("assets/export");
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

void FGodotExportPipeline::WriteGodotGltfImportFile(const FString& GltfAbsolutePath, UObject* MeshObject) const
{
	const FString Ext = FPaths::GetExtension(GltfAbsolutePath, false).ToLower();
	if (Ext != TEXT("glb") && Ext != TEXT("gltf"))
	{
		return;
	}

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(GltfAbsolutePath), true);

	const FString ResPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, GltfAbsolutePath);
	FString Contents;
	Contents += TEXT("[remap]\n\n");
	Contents += TEXT("importer=\"scene\"\n");
	Contents += TEXT("type=\"PackedScene\"\n\n");
	Contents += TEXT("[deps]\n\n");
	Contents += FString::Printf(TEXT("source_file=\"%s\"\n\n"), *ResPath);
	Contents += TEXT("[params]\n\n");
	Contents += TEXT("gltf/naming_version=2\n");
	Contents += TEXT("materials/extract=0\n");
	if (MeshObject)
	{
		// Mesh glb has no images. 0 = DISCARD so Godot does not write PNGs in meshes/.
		Contents += TEXT("gltf/embedded_image_handling=0\n");
	}
	else
	{
		// Level glb keeps baked PBR inside the file (3 = EMBED_AS_UNCOMPRESSED).
		Contents += TEXT("gltf/embedded_image_handling=3\n");
	}

	TArray<UMaterialInterface*> Materials;
	if (MeshObject)
	{
		CollectMeshMaterials(MeshObject, Materials);
	}
	if (Materials.Num() > 0)
	{
		Contents += TEXT("_subresources={\n");
		Contents += TEXT("\"materials\": {\n");
		for (int32 Index = 0; Index < Materials.Num(); ++Index)
		{
			UMaterialInterface* Material = Materials[Index];
			if (!Material)
			{
				continue;
			}

			FString TresPath;
			if (const FString* Existing = ExportedResPaths.Find(Material->GetOutermost()->GetFName()))
			{
				TresPath = *Existing;
			}
			else
			{
				TresPath = PackageToResPath(Material->GetOutermost()->GetName(), TEXT("tres"), Settings, FAssetData(Material));
			}

			const FString MatName = GodotExportPrivate::EscapeJson(Material->GetName());
			const FString TresEscaped = GodotExportPrivate::EscapeJson(TresPath);
			Contents += FString::Printf(TEXT("\"%s\": {\n"), *MatName);
			Contents += TEXT("\"use_external/enabled\": true,\n");
			Contents += FString::Printf(TEXT("\"use_external/path\": \"%s\"\n"), *TresEscaped);
			Contents += (Index + 1 < Materials.Num()) ? TEXT("},\n") : TEXT("}\n");
		}
		Contents += TEXT("}\n");
		Contents += TEXT("}\n");
	}

	FFileHelper::SaveStringToFile(Contents, *(GltfAbsolutePath + TEXT(".import")), FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

void FGodotExportPipeline::GatherMaterialTextureResPaths(UMaterialInterface* Material, TMap<FString, FString>& OutRoleToRes)
{
	if (!Material)
	{
		return;
	}

	auto ExportIfNeeded = [this](UTexture* Texture) -> FString
	{
		if (!Texture || GodotExportPrivate::IsEngineTexture(Texture))
		{
			return FString();
		}
		if (const FString* Existing = ExportedResPaths.Find(Texture->GetOutermost()->GetFName()))
		{
			return *Existing;
		}
		UTexture2D* Tex2D = Cast<UTexture2D>(Texture);
		if (!Tex2D)
		{
			return FString();
		}
		FGodotExportItemResult Nested;
		if (ExportTexture(Tex2D, FAssetData(Tex2D), Nested)
			&& (Nested.Status == EGodotExportStatus::Succeeded || Nested.Status == EGodotExportStatus::Skipped))
		{
			return Nested.OutputPath;
		}
		return FString();
	};

	TMap<FString, TPair<int32, UTexture*>> BestByRole;
	auto Consider = [&](const FString& Role, UTexture* Texture, int32 Score)
	{
		if (Role.IsEmpty() || !Texture || GodotExportPrivate::IsEngineTexture(Texture))
		{
			return;
		}
		if (const TPair<int32, UTexture*>* Existing = BestByRole.Find(Role))
		{
			if (Score <= Existing->Key)
			{
				return;
			}
		}
		BestByRole.Add(Role, TPair<int32, UTexture*>(Score, Texture));
	};

	auto RoleFromParam = [](const FString& ParamName, const UTexture* Texture) -> FString
	{
		FString Role = GodotExportPrivate::GuessTextureRole(ParamName);
		if (Role.IsEmpty() && Texture)
		{
			Role = GodotExportPrivate::GuessTextureRole(Texture->GetName());
		}
		if (Role.IsEmpty() && Texture && Texture->CompressionSettings == TC_Normalmap)
		{
			Role = TEXT("normal");
		}
		return Role;
	};

	TArray<UTexture*> UnnamedInstanceTextures;

	{
		UMaterialInterface* Current = Material;
		int32 Depth = 0;
		while (UMaterialInstance* Inst = Cast<UMaterialInstance>(Current))
		{
			if (Inst->Parent)
			{
				Inst->Parent->ConditionalPostLoad();
			}
			const int32 ScoreBase = 300 - Depth * 10;
			for (const FTextureParameterValue& Param : Inst->TextureParameterValues)
			{
				UTexture* Texture = Param.ParameterValue;
				if (!Texture)
				{
					continue;
				}
				const FString ParamName = Param.ParameterInfo.Name.ToString();
				FString Role = RoleFromParam(ParamName, Texture);
				if (Role.IsEmpty() && Depth == 0)
				{
					UnnamedInstanceTextures.AddUnique(Texture);
				}
				if (!Role.IsEmpty())
				{
					Consider(Role, Texture, ScoreBase + GodotExportPrivate::AlbedoNameScore(ParamName));
				}
				UE_LOG(
					LogGodotExporter,
					Display,
					TEXT("MIC %s override '%s' -> %s (%s) depth=%d"),
					*Material->GetName(),
					*ParamName,
					*Role,
					*Texture->GetName(),
					Depth);
			}
			Current = Inst->Parent;
			if (++Depth > 16)
			{
				break;
			}
		}
	}

	// GetTextureParameterValue walks parent chain; keep as fallback for params without a stored override entry.
	TArray<FMaterialParameterInfo> TextureInfos;
	TArray<FGuid> TextureIds;
	Material->GetAllTextureParameterInfo(TextureInfos, TextureIds);
	for (const FMaterialParameterInfo& Info : TextureInfos)
	{
		UTexture* Texture = nullptr;
		if (!Material->GetTextureParameterValue(Info, Texture) || !Texture)
		{
			continue;
		}
		const FString ParamName = Info.Name.ToString();
		const FString Role = RoleFromParam(ParamName, Texture);
		const int32 Score = 100 + GodotExportPrivate::AlbedoNameScore(ParamName);
		Consider(Role, Texture, Score);
		UE_LOG(
			LogGodotExporter,
			Display,
			TEXT("Material %s tex param '%s' -> %s (%s)"),
			*Material->GetName(),
			*ParamName,
			*Role,
			Texture ? *Texture->GetName() : TEXT("null"));
	}

	const uint32 BitAlbedo = 1u << 0;
	const uint32 BitNormal = 1u << 1;
	const uint32 BitMetallic = 1u << 2;
	const uint32 BitRoughness = 1u << 3;
	const uint32 BitAO = 1u << 4;
	const uint32 BitEmissive = 1u << 5;
	const uint32 BitOpacity = 1u << 6;

	const TPair<EMaterialProperty, uint32> Chains[] = {
		{ MP_BaseColor, BitAlbedo },
		{ MP_Normal, BitNormal },
		{ MP_Metallic, BitMetallic },
		{ MP_Roughness, BitRoughness },
		{ MP_AmbientOcclusion, BitAO },
		{ MP_EmissiveColor, BitEmissive },
		{ MP_Opacity, BitOpacity },
		{ MP_OpacityMask, BitOpacity },
	};

	for (const TPair<EMaterialProperty, uint32>& Chain : Chains)
	{
		TArray<UTexture*> Textures;
		TArray<FName> ParamNames;
		Material->GetTexturesInPropertyChain(Chain.Key, Textures, &ParamNames, nullptr);
		for (const FName& ParamName : ParamNames)
		{
			UTexture* Texture = nullptr;
			if (!Material->GetTextureParameterValue(FMaterialParameterInfo(ParamName), Texture) || !Texture)
			{
				continue;
			}
			const FString Role = RoleFromParam(ParamName.ToString(), Texture);
			Consider(Role, Texture, 90);
		}
		for (UTexture* Texture : Textures)
		{
			FString Role;
			if (Chain.Value == BitNormal || (Texture && Texture->CompressionSettings == TC_Normalmap))
			{
				Role = TEXT("normal");
			}
			else if (Chain.Value == BitAlbedo)
			{
				Role = TEXT("albedo");
			}
			else if (Chain.Value == BitEmissive)
			{
				Role = TEXT("emission");
			}
			else if (Chain.Value == BitAO)
			{
				Role = TEXT("ao");
			}
			else if (Chain.Value == BitRoughness)
			{
				Role = TEXT("roughness");
			}
			else if (Chain.Value == BitMetallic)
			{
				Role = TEXT("metallic");
			}
			else if (Chain.Value == BitOpacity)
			{
				Role = TEXT("alpha");
			}
			Consider(Role, Texture, 10);
		}
	}

	if (!BestByRole.Contains(TEXT("albedo")) && UnnamedInstanceTextures.Num() > 0)
	{
		Consider(TEXT("albedo"), UnnamedInstanceTextures[0], 5);
	}

	for (const TPair<FString, TPair<int32, UTexture*>>& Pair : BestByRole)
	{
		const FString ResPath = ExportIfNeeded(Pair.Value.Value);
		if (!ResPath.IsEmpty())
		{
			OutRoleToRes.Add(Pair.Key, ResPath);
		}
	}
}

bool FGodotExportPipeline::PatchGltfWithExternalTextures(const FString& GltfAbsolutePath, UObject* MeshObject)
{
	if (!MeshObject || !FPaths::FileExists(GltfAbsolutePath))
	{
		return false;
	}
	if (!FPaths::GetExtension(GltfAbsolutePath, false).Equals(TEXT("gltf"), ESearchCase::IgnoreCase))
	{
		return false;
	}

	FString JsonStr;
	if (!FFileHelper::LoadFileToString(JsonStr, *GltfAbsolutePath))
	{
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		return false;
	}

	TArray<UMaterialInterface*> Materials;
	CollectMeshMaterials(MeshObject, Materials);
	if (Materials.Num() == 0)
	{
		return true;
	}

	TMap<FString, UMaterialInterface*> MatsByName;
	TMap<FString, TArray<UMaterialInterface*>> MatsByParentName;
	for (UMaterialInterface* Material : Materials)
	{
		if (!Material)
		{
			continue;
		}
		MatsByName.Add(Material->GetName(), Material);
		if (UMaterialInstance* Inst = Cast<UMaterialInstance>(Material))
		{
			if (Inst->Parent)
			{
				MatsByParentName.FindOrAdd(Inst->Parent->GetName()).AddUnique(Material);
			}
		}
	}

	TArray<UMaterialInterface*> SlotMaterials;
	if (USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(MeshObject))
	{
		for (const FSkeletalMaterial& Slot : SkeletalMesh->GetMaterials())
		{
			SlotMaterials.Add(Slot.MaterialInterface);
		}
	}
	else if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(MeshObject))
	{
		for (const FStaticMaterial& Slot : StaticMesh->GetStaticMaterials())
		{
			SlotMaterials.Add(Slot.MaterialInterface);
		}
	}

	TArray<TSharedPtr<FJsonValue>> Images;
	TArray<TSharedPtr<FJsonValue>> Textures;
	if (Root->HasTypedField<EJson::Array>(TEXT("images")))
	{
		Images = Root->GetArrayField(TEXT("images"));
	}
	if (Root->HasTypedField<EJson::Array>(TEXT("textures")))
	{
		Textures = Root->GetArrayField(TEXT("textures"));
	}

	auto ResPathToUri = [this, &GltfAbsolutePath](const FString& ResPath) -> FString
	{
		FString Relative = ResPath;
		Relative.RemoveFromStart(TEXT("res://"));
		const FString Absolute = FPaths::ConvertRelativePathToFull(FPaths::Combine(GodotProject, Relative));
		FString Uri = Absolute;
		FString Base = FPaths::GetPath(GltfAbsolutePath);
		if (!Base.EndsWith(TEXT("/")) && !Base.EndsWith(TEXT("\\")))
		{
			Base += TEXT("/");
		}
		FPaths::MakePathRelativeTo(Uri, *Base);
		Uri.ReplaceInline(TEXT("\\"), TEXT("/"));
		return Uri;
	};

	auto AddImage = [&Images](const FString& Uri) -> int32
	{
		for (int32 Index = 0; Index < Images.Num(); ++Index)
		{
			const TSharedPtr<FJsonObject> Existing = Images[Index]->AsObject();
			if (Existing.IsValid() && Existing->GetStringField(TEXT("uri")) == Uri)
			{
				return Index;
			}
		}
		TSharedPtr<FJsonObject> Image = MakeShared<FJsonObject>();
		Image->SetStringField(TEXT("uri"), Uri);
		Images.Add(MakeShared<FJsonValueObject>(Image));
		return Images.Num() - 1;
	};

	auto AddTexture = [&Textures](int32 ImageIndex) -> int32
	{
		for (int32 Index = 0; Index < Textures.Num(); ++Index)
		{
			const TSharedPtr<FJsonObject> Existing = Textures[Index]->AsObject();
			if (Existing.IsValid() && static_cast<int32>(Existing->GetNumberField(TEXT("source"))) == ImageIndex)
			{
				return Index;
			}
		}
		TSharedPtr<FJsonObject> Texture = MakeShared<FJsonObject>();
		Texture->SetNumberField(TEXT("source"), ImageIndex);
		Textures.Add(MakeShared<FJsonValueObject>(Texture));
		return Textures.Num() - 1;
	};

	if (!Root->HasTypedField<EJson::Array>(TEXT("materials")))
	{
		return true;
	}

	TArray<TSharedPtr<FJsonValue>> JsonMaterials = Root->GetArrayField(TEXT("materials"));
	for (int32 JsonIndex = 0; JsonIndex < JsonMaterials.Num(); ++JsonIndex)
	{
		const TSharedPtr<FJsonValue>& MatValue = JsonMaterials[JsonIndex];
		TSharedPtr<FJsonObject> JsonMat = MatValue->AsObject();
		if (!JsonMat.IsValid())
		{
			continue;
		}

		const FString MatName = JsonMat->GetStringField(TEXT("name"));
		UMaterialInterface* Material = MatsByName.FindRef(MatName);
		if (!Material)
		{
			for (const TPair<FString, UMaterialInterface*>& Pair : MatsByName)
			{
				if (MatName.StartsWith(Pair.Key) || Pair.Key.StartsWith(MatName))
				{
					Material = Pair.Value;
					break;
				}
			}
		}
		// Unreal glTF often names the material after the parent UMaterial, not the MIC.
		if (!Material)
		{
			if (const TArray<UMaterialInterface*>* Hits = MatsByParentName.Find(MatName))
			{
				if (Hits->Num() == 1)
				{
					Material = (*Hits)[0];
				}
			}
		}
		if (!Material)
		{
			for (const TPair<FString, TArray<UMaterialInterface*>>& Pair : MatsByParentName)
			{
				if (Pair.Value.Num() == 1 && (MatName.StartsWith(Pair.Key) || Pair.Key.StartsWith(MatName)))
				{
					Material = Pair.Value[0];
					break;
				}
			}
		}
		if (!Material && SlotMaterials.IsValidIndex(JsonIndex))
		{
			Material = SlotMaterials[JsonIndex];
		}
		if (!Material)
		{
			continue;
		}

		JsonMat->SetStringField(TEXT("name"), Material->GetName());

		TMap<FString, FString> RoleToRes;
		GatherMaterialTextureResPaths(Material, RoleToRes);

		TSharedPtr<FJsonObject> Pbr = JsonMat->HasTypedField<EJson::Object>(TEXT("pbrMetallicRoughness"))
			? JsonMat->GetObjectField(TEXT("pbrMetallicRoughness"))
			: MakeShared<FJsonObject>();

		if (const FString* Albedo = RoleToRes.Find(TEXT("albedo")))
		{
			const int32 TexIndex = AddTexture(AddImage(ResPathToUri(*Albedo)));
			TSharedPtr<FJsonObject> Tex = MakeShared<FJsonObject>();
			Tex->SetNumberField(TEXT("index"), TexIndex);
			Pbr->SetObjectField(TEXT("baseColorTexture"), Tex);
		}

		{
			const FLinearColor AlbedoColor = GodotExportPrivate::ResolveAlbedoColor(Material);
			TArray<TSharedPtr<FJsonValue>> Factor;
			Factor.Add(MakeShared<FJsonValueNumber>(AlbedoColor.R));
			Factor.Add(MakeShared<FJsonValueNumber>(AlbedoColor.G));
			Factor.Add(MakeShared<FJsonValueNumber>(AlbedoColor.B));
			Factor.Add(MakeShared<FJsonValueNumber>(AlbedoColor.A));
			Pbr->SetArrayField(TEXT("baseColorFactor"), Factor);
		}
		if (const FString* Orm = RoleToRes.Find(TEXT("orm")))
		{
			const int32 TexIndex = AddTexture(AddImage(ResPathToUri(*Orm)));
			TSharedPtr<FJsonObject> Tex = MakeShared<FJsonObject>();
			Tex->SetNumberField(TEXT("index"), TexIndex);
			Pbr->SetObjectField(TEXT("metallicRoughnessTexture"), Tex);
			TSharedPtr<FJsonObject> Occ = MakeShared<FJsonObject>();
			Occ->SetNumberField(TEXT("index"), TexIndex);
			JsonMat->SetObjectField(TEXT("occlusionTexture"), Occ);
		}
		JsonMat->SetObjectField(TEXT("pbrMetallicRoughness"), Pbr);

		if (const FString* Normal = RoleToRes.Find(TEXT("normal")))
		{
			const int32 TexIndex = AddTexture(AddImage(ResPathToUri(*Normal)));
			TSharedPtr<FJsonObject> Tex = MakeShared<FJsonObject>();
			Tex->SetNumberField(TEXT("index"), TexIndex);
			JsonMat->SetObjectField(TEXT("normalTexture"), Tex);
		}
	}

	if (Images.Num() > 0)
	{
		Root->SetArrayField(TEXT("images"), Images);
		Root->SetArrayField(TEXT("textures"), Textures);
	}
	Root->SetArrayField(TEXT("materials"), JsonMaterials);

	FString OutJson;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	if (!FJsonSerializer::Serialize(Root.ToSharedRef(), Writer))
	{
		return false;
	}
	return FFileHelper::SaveStringToFile(OutJson, *GltfAbsolutePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

bool FGodotExportPipeline::PackGltfAndBinToGlb(const FString& GltfAbsolutePath, const FString& GlbAbsolutePath)
{
	FString JsonStr;
	if (!FFileHelper::LoadFileToString(JsonStr, *GltfAbsolutePath))
	{
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonStr);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		return false;
	}

	FString BinPath = FPaths::ChangeExtension(GltfAbsolutePath, TEXT("bin"));
	if (Root->HasTypedField<EJson::Array>(TEXT("buffers")))
	{
		const TArray<TSharedPtr<FJsonValue>>& Buffers = Root->GetArrayField(TEXT("buffers"));
		if (Buffers.Num() > 0)
		{
			const TSharedPtr<FJsonObject> Buffer0 = Buffers[0]->AsObject();
			if (Buffer0.IsValid() && Buffer0->HasField(TEXT("uri")))
			{
				const FString Uri = Buffer0->GetStringField(TEXT("uri"));
				if (!Uri.StartsWith(TEXT("data:")))
				{
					BinPath = FPaths::Combine(FPaths::GetPath(GltfAbsolutePath), Uri);
					FPaths::NormalizeFilename(BinPath);
				}
			}
		}
	}

	TArray<uint8> BinBytes;
	if (FPaths::FileExists(BinPath))
	{
		FFileHelper::LoadFileToArray(BinBytes, *BinPath);
	}

	if (Root->HasTypedField<EJson::Array>(TEXT("buffers")))
	{
		TArray<TSharedPtr<FJsonValue>> Buffers = Root->GetArrayField(TEXT("buffers"));
		if (Buffers.Num() > 0)
		{
			TSharedPtr<FJsonObject> Buffer0 = Buffers[0]->AsObject();
			if (Buffer0.IsValid())
			{
				Buffer0->RemoveField(TEXT("uri"));
				Buffer0->SetNumberField(TEXT("byteLength"), BinBytes.Num());
			}
			Root->SetArrayField(TEXT("buffers"), Buffers);
		}
	}

	FString OutJson;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	if (!FJsonSerializer::Serialize(Root.ToSharedRef(), Writer))
	{
		return false;
	}

	FTCHARToUTF8 Utf8(*OutJson);
	TArray<uint8> JsonBytes;
	JsonBytes.Append(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
	while (JsonBytes.Num() % 4 != 0)
	{
		JsonBytes.Add(static_cast<uint8>(' '));
	}
	while (BinBytes.Num() % 4 != 0)
	{
		BinBytes.Add(0);
	}

	auto AppendU32 = [](TArray<uint8>& Out, uint32 Value)
	{
		Out.Add(static_cast<uint8>(Value));
		Out.Add(static_cast<uint8>(Value >> 8));
		Out.Add(static_cast<uint8>(Value >> 16));
		Out.Add(static_cast<uint8>(Value >> 24));
	};

	const uint32 JsonLen = static_cast<uint32>(JsonBytes.Num());
	const uint32 BinLen = static_cast<uint32>(BinBytes.Num());
	const uint32 Total = 12 + 8 + JsonLen + (BinLen > 0 ? 8 + BinLen : 0);

	TArray<uint8> Glb;
	Glb.Reserve(Total);
	AppendU32(Glb, 0x46546C67); // glTF
	AppendU32(Glb, 2);
	AppendU32(Glb, Total);
	AppendU32(Glb, JsonLen);
	AppendU32(Glb, 0x4E4F534A); // JSON
	Glb.Append(JsonBytes);
	if (BinLen > 0)
	{
		AppendU32(Glb, BinLen);
		AppendU32(Glb, 0x004E4942); // BIN
		Glb.Append(BinBytes);
	}

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(GlbAbsolutePath), true);
	if (!FFileHelper::SaveArrayToFile(Glb, *GlbAbsolutePath))
	{
		return false;
	}

	IFileManager::Get().Delete(*GltfAbsolutePath);
	IFileManager::Get().Delete(*(GltfAbsolutePath + TEXT(".import")));
	if (FPaths::FileExists(BinPath))
	{
		IFileManager::Get().Delete(*BinPath);
	}
	return true;
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
	const bool bAnim = Object->IsA<UAnimSequence>();
	const FString Extension = (Settings->bUseBinaryGlb || !bAnim) ? TEXT("glb") : TEXT("gltf");
	const FString AbsolutePath = ResolveOutputPath(AssetData, Extension);
	Item.OutputPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, AbsolutePath);

	const FString GltfPath = FPaths::ChangeExtension(AbsolutePath, TEXT("gltf"));
	const FString BinPath = FPaths::ChangeExtension(AbsolutePath, TEXT("bin"));
	const bool bHasOrphanSidecars = FPaths::FileExists(GltfPath) || FPaths::FileExists(BinPath);
	if (bAnim && ShouldSkipUnchanged(AssetData, AbsolutePath) && !bHasOrphanSidecars)
	{
		Item.Status = EGodotExportStatus::Skipped;
		Item.Message = TEXT("Destination is up to date");
		ExportedResPaths.Add(AssetData.PackageName, Item.OutputPath);
		return true;
	}

	if (!bAnim)
	{
		const FString SavedOverride = OutputPathOverride;
		OutputPathOverride.Empty();
		ExportMeshSidecars(Object);
		OutputPathOverride = SavedOverride;
		WriteGodotGltfImportFile(AbsolutePath, Object);
	}

	FString Message;
	const FString ExportPath = bAnim ? AbsolutePath : GltfPath;
	if (!ExportWithGltf(Object, ExportPath, Message))
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = Message;
		return false;
	}

	CleanupGltfImagesInMeshFolder(ExportPath);
	if (!bAnim)
	{
		PatchGltfWithExternalTextures(GltfPath, Object);
		if (!PackGltfAndBinToGlb(GltfPath, AbsolutePath))
		{
			Item.Status = EGodotExportStatus::Failed;
			Item.Message = TEXT("Failed to pack .gltf/.bin into .glb");
			return false;
		}
		WriteGodotGltfImportFile(AbsolutePath, Object);
	}
	ExportedResPaths.Add(AssetData.PackageName, Item.OutputPath);
	Item.Status = EGodotExportStatus::Succeeded;
	if (Object->IsA<UAnimSequence>())
	{
		Item.Message = TEXT("Exported animation (no mesh, textures or materials)");
	}
	else
	{
		FString PrefabPath;
		if (WriteMeshPrefab(Object, AssetData, AbsolutePath, Item.OutputPath, PrefabPath))
		{
			Item.Message = TEXT("Exported mesh .glb, materials .tres, prefab .tscn");
		}
		else
		{
			Item.Message = TEXT("Exported mesh .glb (geometry) referencing textures/; materials .tres");
		}
	}
	return true;
}

bool FGodotExportPipeline::WriteMeshPrefab(
	UObject* MeshObject,
	const FAssetData& AssetData,
	const FString& MeshAbsolutePath,
	const FString& MeshResPath,
	FString& OutPrefabPath) const
{
	if (!Settings || !Settings->bExportPrefabs || !MeshObject)
	{
		return false;
	}
	if (MeshObject->IsA<UAnimSequence>())
	{
		return false;
	}

	FString PrefabAbs;
	FString MeshPathForScene = MeshResPath;
	if (bFlattenSidecars || !OutputPathOverride.IsEmpty())
	{
		PrefabAbs = FPaths::ChangeExtension(MeshAbsolutePath, TEXT("tscn"));
		MeshPathForScene = FPaths::GetCleanFilename(MeshAbsolutePath);
	}
	else
	{
		PrefabAbs = PackageToAbsolutePath(GodotProject, AssetData.PackageName.ToString(), TEXT("tscn"), Settings, AssetData);
	}

	const FString NodeName = GodotExportPrivate::SanitizeGodotNodeName(AssetData.AssetName.ToString());

	FString Contents;
	Contents += TEXT("[gd_scene load_steps=2 format=3]\n\n");
	Contents += FString::Printf(TEXT("[ext_resource type=\"PackedScene\" path=\"%s\" id=\"1_mesh\"]\n\n"), *MeshPathForScene);
	Contents += FString::Printf(TEXT("[node name=\"%s\" type=\"Node3D\"]\n\n"), *NodeName);
	Contents += TEXT("[node name=\"Model\" parent=\".\" instance=ExtResource(\"1_mesh\")]\n");

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(PrefabAbs), true);
	if (!FFileHelper::SaveStringToFile(Contents, *PrefabAbs, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogGodotExporter, Warning, TEXT("Failed to write prefab %s"), *PrefabAbs);
		return false;
	}

	OutPrefabPath = GodotExportPrivate::ToResPathFromAbsolute(GodotProject, PrefabAbs);
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

	TMap<FString, FString> RoleToRes;
	GatherMaterialTextureResPaths(Material, RoleToRes);

	TMap<FString, float> ScalarParams;
	{
		TArray<FMaterialParameterInfo> ScalarInfos;
		TArray<FGuid> ScalarIds;
		Material->GetAllScalarParameterInfo(ScalarInfos, ScalarIds);
		for (const FMaterialParameterInfo& Info : ScalarInfos)
		{
			float Value = 0.f;
			if (Material->GetScalarParameterValue(Info, Value))
			{
				ScalarParams.Add(Info.Name.ToString(), Value);
			}
		}
	}

	const FLinearColor Albedo = GodotExportPrivate::ResolveAlbedoColor(Material);

	float Metallic = 0.f;
	float Roughness = 1.f;
	bool bGotMetallic = false;
	bool bGotRoughness = false;
	{
		UMaterialInterface* Current = Material;
		int32 Depth = 0;
		while (UMaterialInstance* Inst = Cast<UMaterialInstance>(Current))
		{
			for (const FScalarParameterValue& Param : Inst->ScalarParameterValues)
			{
				const FString Role = GodotExportPrivate::GuessTextureRole(Param.ParameterInfo.Name.ToString());
				if (Role == TEXT("metallic") && !bGotMetallic)
				{
					Metallic = Param.ParameterValue;
					bGotMetallic = true;
				}
				else if (Role == TEXT("roughness") && !bGotRoughness)
				{
					Roughness = Param.ParameterValue;
					bGotRoughness = true;
				}
			}
			if (bGotMetallic && bGotRoughness)
			{
				break;
			}
			Current = Inst->Parent;
			if (++Depth > 16)
			{
				break;
			}
		}
	}
	if (!bGotMetallic || !bGotRoughness)
	{
		for (const TPair<FString, float>& Pair : ScalarParams)
		{
			const FString Role = GodotExportPrivate::GuessTextureRole(Pair.Key);
			if (Role == TEXT("metallic") && !bGotMetallic)
			{
				Metallic = Pair.Value;
				bGotMetallic = true;
			}
			else if (Role == TEXT("roughness") && !bGotRoughness)
			{
				Roughness = Pair.Value;
				bGotRoughness = true;
			}
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
		WriteGodotGltfImportFile(AbsolutePath);
		return true;
	}

	World = GodotExportPrivate::ResolveWorldForExport(World);
	if (!World || !World->PersistentLevel)
	{
		Item.Status = EGodotExportStatus::Failed;
		Item.Message = TEXT("Open the level in the editor before exporting (the map is not fully loaded)");
		return false;
	}

	FString Message;
	{
		GodotExportPrivate::FScopedClearLightIesProfiles ClearIes(World);
		if (!ExportWithGltf(World, AbsolutePath, Message))
		{
			Item.Status = EGodotExportStatus::Failed;
			Item.Message = Message;
			return false;
		}
	}

	CleanupGltfImagesInMeshFolder(AbsolutePath);
	WriteGodotGltfImportFile(AbsolutePath);

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
