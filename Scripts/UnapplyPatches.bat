@echo off
REM Usage: UnapplyPatches.bat <project_dir>
REM Removes all applied engine patches without re-applying them.
"%~dp0..\Tools\Bin\EnginePatchCLI.exe" %1 --unapply %2 %3 %4 %5
if %ERRORLEVEL% neq 0 (
    echo [EnginePatch] Patch unapply failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)
