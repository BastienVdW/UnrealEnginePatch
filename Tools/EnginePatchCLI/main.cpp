#include "EnginePatchTypes.h"
#include "EnginePatchManager.h"
#include "EnginePatchFileLoader.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

using json = nlohmann::json;
namespace fs = std::filesystem;

#ifdef _WIN32
std::string GetEngineDir(const std::string& version) {
    // First check environment variable
    const char* envDir = std::getenv("UE_ENGINE_DIR");
    if (envDir && *envDir) {
        return std::string(envDir);
    }

    // Then check Windows registry
    std::string keyPath = "SOFTWARE\\EpicGames\\Unreal Engine\\" + version;
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return "";
    }
    char buf[MAX_PATH] = {};
    DWORD bufSize = sizeof(buf);
    RegQueryValueExA(hKey, "InstalledDirectory", nullptr, nullptr, (LPBYTE)buf, &bufSize);
    RegCloseKey(hKey);
    return buf;
}
#endif

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: EnginePatchCLI <project_dir>" << std::endl;
        return 1;
    }

    std::string projectDir = argv[1];
    fs::path projectPath(projectDir);

    // 1. Find the .uproject file
    std::string uprojectPath;
    try {
        for (const auto& entry : fs::directory_iterator(projectPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".uproject") {
                uprojectPath = entry.path().string();
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[EnginePatch] Error scanning project directory: " << e.what() << std::endl;
        return 1;
    }

    if (uprojectPath.empty()) {
        std::cerr << "[EnginePatch] No .uproject file found in " << projectDir << std::endl;
        return 1;
    }

    // 2. Parse the .uproject JSON
    std::string engineVersion;
    std::map<std::string, bool> pluginEnabled;

    try {
        std::ifstream uprojectFile(uprojectPath);
        if (!uprojectFile.is_open()) {
            std::cerr << "[EnginePatch] Failed to open .uproject file: " << uprojectPath << std::endl;
            return 1;
        }

        json uprojectJson = json::parse(uprojectFile);

        // Extract EngineAssociation
        if (uprojectJson.contains("EngineAssociation")) {
            engineVersion = uprojectJson["EngineAssociation"].get<std::string>();
        } else {
            std::cerr << "[EnginePatch] EngineAssociation not found in .uproject" << std::endl;
            return 1;
        }

        // Parse Plugins array
        if (uprojectJson.contains("Plugins") && uprojectJson["Plugins"].is_array()) {
            for (const auto& plugin : uprojectJson["Plugins"]) {
                if (plugin.contains("Name") && plugin.contains("Enabled")) {
                    std::string pluginName = plugin["Name"].get<std::string>();
                    bool enabled = plugin["Enabled"].get<bool>();
                    pluginEnabled[pluginName] = enabled;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[EnginePatch] Failed to parse .uproject: " << e.what() << std::endl;
        return 1;
    }

    // 3. Find the engine directory
    std::string engineDir;
#ifdef _WIN32
    engineDir = GetEngineDir(engineVersion);
    if (engineDir.empty()) {
        std::cerr << "[EnginePatch] Failed to find Unreal Engine " << engineVersion << std::endl;
        return 1;
    }
#else
    // On non-Windows, try environment variable
    const char* envDir = std::getenv("UE_ENGINE_DIR");
    if (!envDir || !*envDir) {
        std::cerr << "[EnginePatch] UE_ENGINE_DIR not set and not on Windows" << std::endl;
        return 1;
    }
    engineDir = std::string(envDir);
#endif

    // 4. Scan for patch JSON files and load them
    std::vector<EnginePatch> allPatches;
    try {
        fs::path pluginsPath = projectPath / "Plugins";
        if (fs::exists(pluginsPath) && fs::is_directory(pluginsPath)) {
            for (const auto& pluginEntry : fs::directory_iterator(pluginsPath)) {
                if (pluginEntry.is_directory()) {
                    fs::path patchDirPath = pluginEntry.path() / "EnginePatch";
                    if (fs::exists(patchDirPath) && fs::is_directory(patchDirPath)) {
                        std::vector<EnginePatch> patchesFromDir =
                            EnginePatchFileLoader::LoadPatchesFromDirectory(patchDirPath.string());
                        allPatches.insert(allPatches.end(), patchesFromDir.begin(), patchesFromDir.end());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[EnginePatch] Error scanning for patch files: " << e.what() << std::endl;
        return 1;
    }

    // 5. Call SyncPatches
    try {
        SyncPatches(allPatches, pluginEnabled, engineDir, engineVersion, std::cout);
    } catch (const std::exception& e) {
        std::cerr << "[EnginePatch] Sync failed: " << e.what() << std::endl;
        return 1;
    }

    // 6. Exit 0 on success
    return 0;
}
