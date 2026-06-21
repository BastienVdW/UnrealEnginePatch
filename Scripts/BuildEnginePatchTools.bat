@echo off
REM Builds EnginePatchCore.lib and EnginePatchCLI.exe and copies them to the committed bin/lib locations.
REM Run this after modifying any source in ThirdParty/EnginePatchCore or Tools/EnginePatchCLI.

set PLUGIN_DIR=%~dp0..
set BUILD_DIR=%PLUGIN_DIR%\build

echo [EnginePatch] Configuring CMake...
cmake -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=Release "%PLUGIN_DIR%"
if %ERRORLEVEL% neq 0 (
    echo [EnginePatch] CMake configuration failed.
    exit /b %ERRORLEVEL%
)

echo [EnginePatch] Building...
cmake --build "%BUILD_DIR%" --config Release
if %ERRORLEVEL% neq 0 (
    echo [EnginePatch] Build failed.
    exit /b %ERRORLEVEL%
)

echo [EnginePatch] Copying binaries...
copy /Y "%BUILD_DIR%\Tools\EnginePatchCLI\Release\EnginePatchCLI.exe" "%PLUGIN_DIR%\Tools\Bin\EnginePatchCLI.exe"
copy /Y "%BUILD_DIR%\ThirdParty\EnginePatchCore\Release\EnginePatchCore.lib" "%PLUGIN_DIR%\ThirdParty\EnginePatchCore\Lib\Win64\EnginePatchCore.lib"

echo [EnginePatch] Done. Commit Tools/Bin/EnginePatchCLI.exe and ThirdParty/EnginePatchCore/Lib/Win64/EnginePatchCore.lib.
