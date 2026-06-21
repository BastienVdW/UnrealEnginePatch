# UnrealEnginePatch

An editor-only Unreal Engine plugin that applies and reverts versioned, line-level patches to stock engine source files. Patches are defined as JSON files and can be tied to specific plugins so they are automatically applied or removed when those plugins are enabled or disabled.

## How it works

- On editor close, the subsystem reads the current project descriptor and for each patch:
  - Plugin **enabled** + patch **not applied** → applies it.
  - Plugin **disabled** + patch **applied** → removes it.
- Patches are idempotent and wrapped in marker comments so they revert cleanly.
- If no `versions` entry matches the running engine the patch is silently skipped.

The **Engine Patch Manager** panel (`Tools > Engine Patch Manager`) shows patch status and lets you apply or revert them individually or all at once.

## Patch file location

```
Plugins/<YourPlugin>/EnginePatch/*.json
```

`UnrealEnginePatch` scans every `Plugins/*/EnginePatch/` directory automatically.

## JSON format

One file per logical patch. A patch can touch multiple engine files with multiple operations per file.

```json
{
  "patchId": "my-patch-id",
  "description": "Human-readable description shown in the panel",
  "plugin": "MyPlugin",
  "versions": [
    {
      "engineVersions": ["5.8"],
      "files": [
        {
          "file": "Runtime/SomeModule/Public/SomeHeader.h",
          "operations": [
            {
              "id": "op-unique-id",
              "line": 42,
              "remove": ["  old line;"],
              "add":    ["  new line A;", "  new line B;"]
            }
          ]
        }
      ]
    }
  ]
}
```

| Field | Required | Description |
|---|---|---|
| `patchId` | Yes | Unique identifier used in marker comments. Must be stable. |
| `plugin` | No | `.uplugin` name (without extension). Drives auto-apply/revert. Omit for unconditional patches. |
| `engineVersions` | Yes | e.g. `["5.7", "5.8"]`. Only the matching entry is applied. |
| `file` | Yes | Path relative to `<EngineDir>/Source/` (or `<EngineDir>/Plugins/` if the path starts with `Plugins/`). |
| `line` | Yes | 1-based line in the **original unpatched** file. The patcher tracks offset across multiple operations automatically. |
| `remove` | No | Lines to remove at `line`. Must match exactly (whitespace included). |
| `add` | No | Lines to insert in place of the removed lines. |

## How patches are stored in the engine file

```cpp
// @@PATCH_BEGIN(my-patch-id::op-unique-id)
// @@REMOVED:   old line;
  new line A;
  new line B;
// @@PATCH_END(my-patch-id::op-unique-id)
```

`@@REMOVED:` preserves the original content so unpatch works without re-reading the JSON.

## Manual patch sync (CLI / batch)

Use `Scripts/SyncPatches.bat` to apply patches outside the editor — useful for CI or testing against a specific engine build.

```
SyncPatches.bat <project_dir> [engine_dir] [--no-reapply]
```

| Argument | Description |
|---|---|
| `project_dir` | Directory containing the `.uproject` file. |
| `engine_dir` | *(Optional)* Path to the engine's `Engine/` folder. When omitted, resolved from `.uproject` `EngineAssociation` via the Windows registry. |
| `--no-reapply` | Only apply missing patches; skip the unpatch+reapply cycle. |

```bat
REM Registered engine (resolved from .uproject)
SyncPatches.bat "<project_dir>"

REM Source-built engine
SyncPatches.bat "<project_dir>" "<engine_root>\Engine"
```

`EnginePatchCLI.exe` also accepts `--engine-dir <path>` directly. `UE_ENGINE_DIR` is a fallback environment variable (checked before the registry).

## Adding patches for a new plugin

1. Create `Plugins/<YourPlugin>/EnginePatch/` and add JSON files following the format above.
2. Set `"plugin": "<YourPlugin>"` so auto-sync works.
3. Build and open the editor — patches appear in `Tools > Engine Patch Manager`.
