@echo off
"%~dp0..\Tools\Bin\EnginePatchCLI.exe" %*
if %ERRORLEVEL% neq 0 (
    echo [EnginePatch] Patch sync failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)
