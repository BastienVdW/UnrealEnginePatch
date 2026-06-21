// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

#include "EnginePatchBridge.h"

// Include lib headers via explicit relative path to avoid ambiguity with UE's EnginePatchTypes.h
#include "../../../ThirdParty/EnginePatchCore/include/EnginePatchManager.h"

#include "Misc/Paths.h"
#include "Runtime/Launch/Resources/Version.h"

#include <string>

// ---- Type conversion helpers ----

static std::string ToStd(const FString& S)
{
	return std::string(TCHAR_TO_UTF8(*S));
}

static EnginePatch ToLibPatch(const FEnginePatch& P)
{
	EnginePatch Lib;
	Lib.patchId     = ToStd(P.PatchId);
	Lib.description = ToStd(P.Description);
	Lib.plugin      = ToStd(P.Plugin);

	for (const FEnginePatchVersion& UEVer : P.Versions)
	{
		PatchVersion LibVer;
		for (const FString& V : UEVer.EngineVersions)
			LibVer.engineVersions.push_back(ToStd(V));

		for (const FEnginePatchFile& UEFile : UEVer.Files)
		{
			PatchFile LibFile;
			LibFile.file = ToStd(UEFile.File);

			for (const FEnginePatchOperation& UEOp : UEFile.Operations)
			{
				PatchOperation LibOp;
				LibOp.id   = ToStd(UEOp.Id);
				LibOp.line = UEOp.Line;
				for (const FString& R : UEOp.Remove) LibOp.remove.push_back(ToStd(R));
				for (const FString& A : UEOp.Add)    LibOp.add.push_back(ToStd(A));
				LibFile.operations.push_back(std::move(LibOp));
			}
			LibVer.files.push_back(std::move(LibFile));
		}
		Lib.versions.push_back(std::move(LibVer));
	}
	return Lib;
}

static std::string GetEngineDir()
{
	FString Dir = FPaths::ConvertRelativePathToFull(FPaths::EngineDir());
	return ToStd(Dir);
}

static std::string GetEngineVersion()
{
	return std::to_string(ENGINE_MAJOR_VERSION) + "." + std::to_string(ENGINE_MINOR_VERSION);
}

static EPatchStatus FromLibStatus(PatchStatus S)
{
	switch (S)
	{
		case PatchStatus::Applied:       return EPatchStatus::Applied;
		case PatchStatus::NotApplied:    return EPatchStatus::NotApplied;
		case PatchStatus::NotApplicable: return EPatchStatus::NotApplicable;
		case PatchStatus::Error:         return EPatchStatus::Error;
		default:                         return EPatchStatus::Unknown;
	}
}

// ---- Bridge implementation ----

EPatchStatus FEnginePatchBridge::GetPatchStatus(const FEnginePatch& Patch)
{
	return FromLibStatus(EnginePatchManager::GetPatchStatus(ToLibPatch(Patch), GetEngineDir(), GetEngineVersion()));
}

bool FEnginePatchBridge::ApplyPatch(const FEnginePatch& Patch, FString& OutError)
{
	std::string Err;
	bool bOk = EnginePatchManager::ApplyPatch(ToLibPatch(Patch), GetEngineDir(), GetEngineVersion(), Err);
	if (!bOk) OutError = UTF8_TO_TCHAR(Err.c_str());
	return bOk;
}

bool FEnginePatchBridge::UnpatchPatch(const FEnginePatch& Patch, FString& OutError)
{
	std::string Err;
	bool bOk = EnginePatchManager::UnpatchPatch(ToLibPatch(Patch), GetEngineDir(), GetEngineVersion(), Err);
	if (!bOk) OutError = UTF8_TO_TCHAR(Err.c_str());
	return bOk;
}
