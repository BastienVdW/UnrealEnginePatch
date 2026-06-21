// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "UnrealEnginePatchSubsystem.h"
#include "EnginePatchFileLoader.h"
#include "EnginePatchManager.h"
#include "Misc/Paths.h"

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
}

void UUnrealEnginePatchSubsystem::Deinitialize()
{
	Super::Deinitialize();
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
