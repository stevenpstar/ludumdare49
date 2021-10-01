// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LudumDare49 : ModuleRules
{
	public LudumDare49(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
