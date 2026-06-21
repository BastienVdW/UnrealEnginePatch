// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#pragma once
#include "EnginePatchTypes.h"

// Bridges UE types (FEnginePatch) to the EnginePatchCore standalone lib.
// All file I/O logic now lives in the lib; this adapter converts types and
// resolves engine dir/version from the UE runtime.
class FEnginePatchBridge
{
public:
	static EPatchStatus GetPatchStatus(const FEnginePatch& Patch);
	static bool ApplyPatch(const FEnginePatch& Patch, FString& OutError);
	static bool UnpatchPatch(const FEnginePatch& Patch, FString& OutError);
};
