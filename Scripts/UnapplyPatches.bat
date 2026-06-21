@echo off
REM Usage: UnapplyPatches.bat <project_dir> [engine_dir]
REM   engine_dir  Optional path to the engine's Engine/ folder.
REM               When omitted the engine is resolved from the .uproject EngineAssociation
REM               via the Windows registry.

set PROJECT_DIR=%~1
set ENGINE_DIR_ARG=

if "%~2"=="" goto run
set SECOND=%~2
if "%SECOND:~0,2%"=="--" goto run
set ENGINE_DIR_ARG=--engine-dir "%~2"

:run
"%~dp0..\Tools\Bin\EnginePatchCLI.exe" "%PROJECT_DIR%" --unapply %ENGINE_DIR_ARG%
if %ERRORLEVEL% neq 0 (
    echo [EnginePatch] Patch unapply failed with error %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)