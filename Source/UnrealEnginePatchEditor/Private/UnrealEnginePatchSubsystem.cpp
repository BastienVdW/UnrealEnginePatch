// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "UnrealEnginePatchSubsystem.h"
#include "EnginePatchFileLoader.h"
#include "EnginePatchManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

static bool IsPluginEnabled(const FString& PluginName)
{
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(PluginName);
	return Plugin.IsValid() && Plugin->IsEnabled();
}

void UUnrealEnginePatchSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Default discovery: Recall plugin's EnginePatch directory
	FString RecallPatchDir = FPaths::ConvertRelativePathToFull(
		FPaths::ProjectPluginsDir() / TEXT("Recall/EnginePatch"));
	PatchDirectories.Add(RecallPatchDir);

	for (const FString& Dir : PatchDirectories)
	{
		TArray<FEnginePatch> Loaded = FEnginePatchFileLoader::LoadPatchesFromDirectory(Dir);
		Patches.Append(MoveTemp(Loaded));
	}

	RefreshStatus();
	SyncPatchesToPluginState();
}

void UUnrealEnginePatchSubsystem::Deinitialize()
{
	// On editor close, unpatch any patch whose plugin is currently disabled
	// so engine source stays clean when that plugin is not in use
	for (FEnginePatch& Patch : Patches)
	{
		if (Patch.Plugin.IsEmpty()) continue;
		if (!IsPluginEnabled(Patch.Plugin) && Patch.Status == EPatchStatus::Applied)
		{
			FString Error;
			FEnginePatchManager::UnpatchPatch(Patch, Error);
		}
	}
	Super::Deinitialize();
}

void UUnrealEnginePatchSubsystem::SyncPatchesToPluginState()
{
	for (FEnginePatch& Patch : Patches)
	{
		const bool bPluginEnabled = Patch.Plugin.IsEmpty() || IsPluginEnabled(Patch.Plugin);
		FString Error;
		if (bPluginEnabled && Patch.Status == EPatchStatus::NotApplied)
		{
			if (!FEnginePatchManager::ApplyPatch(Patch, Error))
			{
				Patch.Status = EPatchStatus::Error;
				Patch.ErrorMessage = Error;
				UE_LOG(LogTemp, Error, TEXT("EnginePatch: auto-apply failed for [%s]: %s"), *Patch.PatchId, *Error);
			}
			else
			{
				Patch.Status = FEnginePatchManager::GetPatchStatus(Patch);
			}
		}
		else if (!bPluginEnabled && Patch.Status == EPatchStatus::Applied)
		{
			if (!FEnginePatchManager::UnpatchPatch(Patch, Error))
			{
				Patch.Status = EPatchStatus::Error;
				Patch.ErrorMessage = Error;
				UE_LOG(LogTemp, Error, TEXT("EnginePatch: auto-unpatch failed for [%s]: %s"), *Patch.PatchId, *Error);
			}
			else
			{
				Patch.Status = FEnginePatchManager::GetPatchStatus(Patch);
			}
		}
	}
}

void UUnrealEnginePatchSubsystem::RefreshStatus()
{
	for (FEnginePatch& Patch : Patches)
	{
		Patch.Status = FEnginePatchManager::GetPatchStatus(Patch);
		Patch.ErrorMessage.Empty();
	}
}

void UUnrealEnginePatchSubsystem::ApplyAll(TArray<FString>& OutErrors)
{
	for (FEnginePatch& Patch : Patches)
	{
		if (Patch.Status == EPatchStatus::Applied || Patch.Status == EPatchStatus::NotApplicable) continue;
		FString Error;
		if (!FEnginePatchManager::ApplyPatch(Patch, Error))
		{
			Patch.Status = EPatchStatus::Error;
			Patch.ErrorMessage = Error;
			OutErrors.Add(FString::Printf(TEXT("[%s] %s"), *Patch.PatchId, *Error));
		}
		else
		{
			Patch.Status = FEnginePatchManager::GetPatchStatus(Patch);
		}
	}
}

void UUnrealEnginePatchSubsystem::UnpatchAll(TArray<FString>& OutErrors)
{
	for (FEnginePatch& Patch : Patches)
	{
		if (Patch.Status == EPatchStatus::NotApplied || Patch.Status == EPatchStatus::NotApplicable) continue;
		FString Error;
		if (!FEnginePatchManager::UnpatchPatch(Patch, Error))
		{
			Patch.Status = EPatchStatus::Error;
			Patch.ErrorMessage = Error;
			OutErrors.Add(FString::Printf(TEXT("[%s] %s"), *Patch.PatchId, *Error));
		}
		else
		{
			Patch.Status = FEnginePatchManager::GetPatchStatus(Patch);
		}
	}
}

bool UUnrealEnginePatchSubsystem::ApplyPatch(const FString& PatchId, FString& OutError)
{
	FEnginePatch* Found = Patches.FindByPredicate([&](const FEnginePatch& P) { return P.PatchId == PatchId; });
	if (!Found) { OutError = TEXT("Patch not found"); return false; }
	bool bOk = FEnginePatchManager::ApplyPatch(*Found, OutError);
	Found->Status = FEnginePatchManager::GetPatchStatus(*Found);
	if (!bOk) Found->Status = EPatchStatus::Error;
	return bOk;
}

bool UUnrealEnginePatchSubsystem::UnpatchPatch(const FString& PatchId, FString& OutError)
{
	FEnginePatch* Found = Patches.FindByPredicate([&](const FEnginePatch& P) { return P.PatchId == PatchId; });
	if (!Found) { OutError = TEXT("Patch not found"); return false; }
	bool bOk = FEnginePatchManager::UnpatchPatch(*Found, OutError);
	Found->Status = FEnginePatchManager::GetPatchStatus(*Found);
	if (!bOk) Found->Status = EPatchStatus::Error;
	return bOk;
}
