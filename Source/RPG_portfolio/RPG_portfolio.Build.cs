// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPG_portfolio : ModuleRules
{
	public RPG_portfolio(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "Json", "JsonUtilities", "PlayFab", "PlayFabCpp", "PlayFabCommon", "HTTP" });
    }
}
