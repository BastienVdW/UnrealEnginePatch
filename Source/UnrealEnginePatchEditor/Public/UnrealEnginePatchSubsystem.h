// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "EnginePatchTypes.h"
#include "UnrealEnginePatchSubsystem.generated.h"

UCLASS()
class UNREALENGINEPATCHEDITOR_API UUnrealEnginePatchSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Directories to scan for patch JSON files. Populated in Initialize().
	TArray<FString> PatchDirectories;

	// All discovered patches for the current session.
	TArray<FEnginePatch> Patches;

	// Refresh status of all patches from disk.
	void RefreshStatus();

	// Apply all patches (skips NotApplicable and already Applied).
	void ApplyAll(TArray<FString>& OutErrors);

	// Revert all patches (skips NotApplicable and already NotApplied).
	void UnpatchAll(TArray<FString>& OutErrors);

	// Apply a single patch by ID.
	bool ApplyPatch(const FString& PatchId, FString& OutError);

	// Revert a single patch by ID.
	bool UnpatchPatch(const FString& PatchId, FString& OutError);
};
