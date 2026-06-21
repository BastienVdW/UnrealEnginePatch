// Copyright (C) 2024 Van de Walle Bastien
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0


#include "EnginePatchFileLoader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

TArray<FEnginePatch> FEnginePatchFileLoader::LoadPatchesFromDirectory(const FString& Directory)
{
	TArray<FEnginePatch> Result;
	TArray<FString> JsonFiles;
	IFileManager::Get().FindFiles(JsonFiles, *(Directory / TEXT("*.json")), true, false);

	for (const FString& Filename : JsonFiles)
	{
		FString FullPath = Directory / Filename;
		FString Content;
		if (!FFileHelper::LoadFileToString(Content, *FullPath))
		{
			UE_LOG(LogTemp, Warning, TEXT("EnginePatch: failed to read %s"), *FullPath);
			continue;
		}

		FEnginePatch Patch;
		FString Error;
		if (ParsePatchJson(Content, Patch, Error))
		{
			Result.Add(MoveTemp(Patch));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("EnginePatch: parse error in %s: %s"), *FullPath, *Error);
		}
	}
	return Result;
}

bool FEnginePatchFileLoader::ParsePatchJson(const FString& JsonContent, FEnginePatch& OutPatch, FString& OutError)
{
	TSharedPtr<FJsonObject> RootObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContent);
	if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
	{
		OutError = TEXT("Invalid JSON");
		return false;
	}

	if (!RootObj->TryGetStringField(TEXT("patchId"), OutPatch.PatchId) ||
		!RootObj->TryGetStringField(TEXT("description"), OutPatch.Description))
	{
		OutError = TEXT("Missing patchId or description");
		return false;
	}
	RootObj->TryGetStringField(TEXT("plugin"), OutPatch.Plugin); // optional

	const TArray<TSharedPtr<FJsonValue>>* VersionsArray;
	if (!RootObj->TryGetArrayField(TEXT("versions"), VersionsArray))
	{
		OutError = TEXT("Missing versions array");
		return false;
	}

	for (const TSharedPtr<FJsonValue>& VersionVal : *VersionsArray)
	{
		const TSharedPtr<FJsonObject>* VersionObj;
		if (!VersionVal->TryGetObject(VersionObj)) continue;

		FEnginePatchVersion Version;
		// Support both "engineVersions": ["5.7","5.8"] and legacy "engineVersion": "5.8"
		const TArray<TSharedPtr<FJsonValue>>* VersionsField;
		if ((*VersionObj)->TryGetArrayField(TEXT("engineVersions"), VersionsField))
		{
			for (const TSharedPtr<FJsonValue>& V : *VersionsField)
				Version.EngineVersions.Add(V->AsString());
		}
		else
		{
			FString SingleVersion;
			if ((*VersionObj)->TryGetStringField(TEXT("engineVersion"), SingleVersion))
				Version.EngineVersions.Add(SingleVersion);
		}

		const TArray<TSharedPtr<FJsonValue>>* FilesArray;
		if (!(*VersionObj)->TryGetArrayField(TEXT("files"), FilesArray)) continue;

		for (const TSharedPtr<FJsonValue>& FileVal : *FilesArray)
		{
			const TSharedPtr<FJsonObject>* FileObj;
			if (!FileVal->TryGetObject(FileObj)) continue;

			FEnginePatchFile PatchFile;
			(*FileObj)->TryGetStringField(TEXT("file"), PatchFile.File);

			const TArray<TSharedPtr<FJsonValue>>* OpsArray;
			if (!(*FileObj)->TryGetArrayField(TEXT("operations"), OpsArray)) continue;

			for (const TSharedPtr<FJsonValue>& OpVal : *OpsArray)
			{
				const TSharedPtr<FJsonObject>* OpObj;
				if (!OpVal->TryGetObject(OpObj)) continue;

				FEnginePatchOperation Op;
				(*OpObj)->TryGetStringField(TEXT("id"), Op.Id);
				(*OpObj)->TryGetNumberField(TEXT("line"), Op.Line);

				const TArray<TSharedPtr<FJsonValue>>* RemoveArr;
				if ((*OpObj)->TryGetArrayField(TEXT("remove"), RemoveArr))
					for (auto& V : *RemoveArr) Op.Remove.Add(V->AsString());

				const TArray<TSharedPtr<FJsonValue>>* AddArr;
				if ((*OpObj)->TryGetArrayField(TEXT("add"), AddArr))
					for (auto& V : *AddArr) Op.Add.Add(V->AsString());

				PatchFile.Operations.Add(MoveTemp(Op));
			}
			Version.Files.Add(MoveTemp(PatchFile));
		}
		OutPatch.Versions.Add(MoveTemp(Version));
	}
	return true;
}
