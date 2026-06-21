@echo off
REM Usage: SyncPatches.bat <project_dir> [engine_dir] [--no-reapply]
REM   engine_dir  Optional path to the engine root (e.g. "D:\Projects\CCR\Engine").
REM               When omitted the engine is resolved from the .uproject EngineAssociation
REM               via the Windows registry.
REM Passes --reapply by default so patches are always re-applied fresh from a .bat call.
REM The UE editor pre-build hook calls the CLI directly without --reapply (incremental only).

set PROJECT_DIR=%~1
set ENGINE_DIR_ARG=
set EXTRA_FLAGS=

if "%~2"=="" goto run
set SECOND=%~2
if "%SECOND:~0,2%"=="--" (
    set EXTRA_FLAGS=%SECOND%
) else (
    set ENGINE_DIR_ARG=--engine-dir "%~2"
)

if "%~3"=="" goto run
set EXTRA_FLAGS=%EXTRA_FLAGS% %~3

:run
"%~dp0..\Tools\Bin\EnginePatchCLI.exe" "%PROJECT_DIR%" --reapply %ENGINE_DIR_ARG% %EXTRA_FLAGS%
if %ERRORLEVEL% neq 0 (
    echo [EnginePatch] Patch sync failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)
