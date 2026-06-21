// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "EnginePatchManager.h"
#include "EnginePatchBridge.h"
#include "Runtime/Launch/Resources/Version.h"

FString FEnginePatchManager::GetCurrentEngineVersion()
{
	return FString::Printf(TEXT("%d.%d"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION);
}

FString FEnginePatchManager::MakeBeginMarker(const FString& PatchId, const FString& OpId)
{
	return FString::Printf(TEXT("// @@PATCH_BEGIN(%s::%s)"), *PatchId, *OpId);
}

FString FEnginePatchManager::MakeEndMarker(const FString& PatchId, const FString& OpId)
{
	return FString::Printf(TEXT("// @@PATCH_END(%s::%s)"), *PatchId, *OpId);
}

EPatchStatus FEnginePatchManager::GetPatchStatus(const FEnginePatch& Patch)
{
	return FEnginePatchBridge::GetPatchStatus(Patch);
}

bool FEnginePatchManager::ApplyPatch(const FEnginePatch& Patch, FString& OutError)
{
	return FEnginePatchBridge::ApplyPatch(Patch, OutError);
}

bool FEnginePatchManager::UnpatchPatch(const FEnginePatch& Patch, FString& OutError)
{
	return FEnginePatchBridge::UnpatchPatch(Patch, OutError);
}
