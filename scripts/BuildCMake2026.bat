@echo off
setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

set "CLEAN_FIRST="
if /I "%~2"=="--clean-first" set "CLEAN_FIRST=--clean-first"

set "BUILD_PRESET=debug"
if /I "%CONFIG%"=="Release" set "BUILD_PRESET=release"

if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="Release" (
    echo [ERR] Unknown configuration: %CONFIG%
    exit /b 1
)

pushd "%~dp0..\EzLaboratory" >nul

call "%~dp0CMake.bat" --preset vs2026-x64
if errorlevel 1 (
    popd >nul
    exit /b 1
)

call "%~dp0CMake.bat" --build --preset %BUILD_PRESET% --target EzLaboratory %CLEAN_FIRST%
set "RESULT=%ERRORLEVEL%"

popd >nul
exit /b %RESULT%
