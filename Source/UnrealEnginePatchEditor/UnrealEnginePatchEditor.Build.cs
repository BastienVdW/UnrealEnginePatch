// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

using UnrealBuildTool;
using System.IO;

public class UnrealEnginePatchEditor : ModuleRules
{
    public UnrealEnginePatchEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // EnginePatchCore standalone lib (ThirdParty)
        string EnginePatchCoreDir = Path.Combine(ModuleDirectory, "../../ThirdParty/EnginePatchCore");
        PrivateIncludePaths.Add(Path.Combine(EnginePatchCoreDir, "include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(EnginePatchCoreDir, "Lib/Win64/EnginePatchCore.lib"));
        }

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "EditorSubsystem",
            "UnrealEd",
            "Json",
            "JsonUtilities",
            "ToolMenus",
            "WorkspaceMenuStructure",
            "InputCore",
            "Projects",
        });
    }
}
