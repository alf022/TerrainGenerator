//Copyright 2020 Marchetti Alfredo. All Rights Reserved.

using UnrealBuildTool;

public class TerrainGeneratorEd : ModuleRules
{
    public TerrainGeneratorEd(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
				
			}
            );


        PrivateIncludePaths.AddRange(
            new string[] {
			    
             
            }
            );
         
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "InputCore",
            }
            );

        //Only editor modules
        if (Target.Type == TargetRules.TargetType.Editor)
        {
        
            PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "UnrealEd",
                "UMG",
                "UMGEditor",
                "Blutility",
                "Kismet",
            }
            );
        }

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine", 
                "Slate",
                "SlateCore",  
                "TerrainGenerator",
                "ToolMenus",
                "Projects",
                "InputCore",
                "EditorFramework",
                "GameplayTags",
                "DeveloperSettings"
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );
    }
}
