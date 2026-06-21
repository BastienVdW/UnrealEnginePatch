// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include "EnginePatchTypes.h"

// Thin UE-facing wrapper. All file I/O is delegated to EnginePatchCore via FEnginePatchBridge.
class FEnginePatchManager
{
public:
	static FString GetCurrentEngineVersion();
	static EPatchStatus GetPatchStatus(const FEnginePatch& Patch);
	static bool ApplyPatch(const FEnginePatch& Patch, FString& OutError);
	static bool UnpatchPatch(const FEnginePatch& Patch, FString& OutError);
	static FString MakeBeginMarker(const FString& PatchId, const FString& OpId);
	static FString MakeEndMarker(const FString& PatchId, const FString& OpId);
};
