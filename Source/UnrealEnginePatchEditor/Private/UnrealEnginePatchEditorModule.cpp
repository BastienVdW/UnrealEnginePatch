// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "UnrealEnginePatchEditorModule.h"
#include "SEnginePatchPanel.h"
#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "UnrealEnginePatchEditor"

static const FName EnginePatchTabName("EnginePatchManager");

void FUnrealEnginePatchEditorModule::StartupModule()
{
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(EnginePatchTabName,
        FOnSpawnTab::CreateRaw(this, &FUnrealEnginePatchEditorModule::OnSpawnPatchTab))
        .SetDisplayName(LOCTEXT("TabTitle", "Engine Patches"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUnrealEnginePatchEditorModule::RegisterMenus));
}

void FUnrealEnginePatchEditorModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EnginePatchTabName);
}

void FUnrealEnginePatchEditorModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
    FToolMenuSection& Section = Menu->FindOrAddSection("UnrealEnginePatch");
    Section.AddMenuEntry(
        "OpenEnginePatchManager",
        LOCTEXT("MenuEntry", "Engine Patch Manager"),
        LOCTEXT("MenuEntryTooltip", "Apply or revert engine source patches"),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateLambda([]()
        {
            FGlobalTabmanager::Get()->TryInvokeTab(FName("EnginePatchManager"));
        }))
    );
}

TSharedPtr<SDockTab> FUnrealEnginePatchEditorModule::OnSpawnPatchTab(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SEnginePatchPanel)
		];
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealEnginePatchEditorModule, UnrealEnginePatchEditor)
