// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ThirdPerson : ModuleRules
{
	public ThirdPerson(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

		PublicIncludePaths.AddRange(
	   new string[] {
				   // ... add public include paths required here ...
				   "ThirdPerson/Character/",
				   "ThirdPerson/GameMode/",
				   "ThirdPerson/Projectile/",
				   "ThirdPerson/GameState/",
				   "ThirdPerson/PlayerController/",
				   "ThirdPerson/PlayerState/",
	   }
	   );
	}
}
