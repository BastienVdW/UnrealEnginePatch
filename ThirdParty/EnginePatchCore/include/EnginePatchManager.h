#pragma once

#include "EnginePatchTypes.h"
#include <string>
#include <vector>

class EnginePatchManager {
public:
    static std::string MakeBeginMarker(const std::string& patchId, const std::string& opId);
    static std::string MakeEndMarker(const std::string& patchId, const std::string& opId);
    static PatchStatus GetPatchStatus(
        const EnginePatch& patch,
        const std::string& engineDir,
        const std::string& engineVersion);
    static bool ApplyPatch(
        const EnginePatch& patch,
        const std::string& engineDir,
        const std::string& engineVersion,
        std::string& outError);
    static bool UnpatchPatch(
        const EnginePatch& patch,
        const std::string& engineDir,
        const std::string& engineVersion,
        std::string& outError);

private:
    static std::string ResolveEnginePath(
        const std::string& engineDir,
        const std::string& relPath);
    static bool IsOperationApplied(
        const std::string& filePath,
        const std::string& patchId,
        const std::string& opId);
    // Counts the net lines added by all already-applied patch blocks whose BEGIN marker
    // appears before originalLine0Based in the current file state.
    static int ComputeExistingPatchOffset(
        const std::vector<std::string>& lines,
        int originalLine0Based);
    static bool ApplyOperation(
        std::vector<std::string>& lines,
        const std::string& patchId,
        const PatchOperation& op,
        std::string& outError);
    static bool UnpatchOperation(
        const std::string& filePath,
        const std::string& patchId,
        const PatchOperation& op,
        std::string& outError);
    static std::string Trim(const std::string& str);
    static void MakeWritable(const std::string& filePath);
};
