// Copyright 2019 Ilgar Lunin. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class GoogleSpeechKit : ModuleRules
{

    public GoogleSpeechKit(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(ModuleDirectory, "Public"),
            }
		);

        PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
            }
			);
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "Voice",
                "Json",
                "XmlParser",
                "Http"
				// ... add other public dependencies that you statically link with here ...
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "Voice"
				// ... add private dependencies that you statically link with here ...	
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
