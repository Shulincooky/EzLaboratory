@echo off
setlocal

set "BUILD_PRESET=debug"
if /I "%~1"=="release" set "BUILD_PRESET=release"

if /I not "%~1"=="" if /I not "%~1"=="debug" if /I not "%~1"=="release" (
    echo [ERR] Usage: build.bat [debug^|release]
    exit /b 1
)

pushd "%~dp0EzLaboratory" >nul

call "%~dp0scripts\CMake.bat" --preset vs2026-x64
if errorlevel 1 (
    popd >nul
    exit /b 1
)

call "%~dp0scripts\CMake.bat" --build --preset %BUILD_PRESET%
set "RESULT=%ERRORLEVEL%"

popd >nul
exit /b %RESULT%
