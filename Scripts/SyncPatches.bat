@echo off
REM Usage: SyncPatches.bat <project_dir> [--no-reapply]
REM Passes --reapply by default so patches are always re-applied fresh from a .bat call.
REM The UE editor pre-build hook calls the CLI directly without --reapply (incremental only).
"%~dp0..\Tools\Bin\EnginePatchCLI.exe" %1 --reapply %2 %3 %4 %5
if %ERRORLEVEL% neq 0 (
    echo [EnginePatch] Patch sync failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)
