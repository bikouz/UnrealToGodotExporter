using UnrealBuildTool;

public class GodotExporter : ModuleRules
{
	public GodotExporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"AssetTools",
			"AssetRegistry",
			"ContentBrowser",
			"ContentBrowserData",
			"Slate",
			"SlateCore",
			"InputCore",
			"AppFramework",
			"ToolMenus",
			"LevelEditor",
			"WorkspaceMenuStructure",
			"Projects",
			"Json",
			"JsonUtilities",
			"DesktopPlatform",
			"ApplicationCore",
			"PropertyEditor",
			"GLTFExporter",
			"PhysicsCore",
			"ImageCore",
			"ImageWrapper",
			"RenderCore",
			"RHI",
			"EditorStyle",
			"EditorWidgets",
			"EditorFramework",
			"EditorSubsystem",
			"ToolWidgets"
		});
	}
}
