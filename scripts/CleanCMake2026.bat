@echo off
setlocal

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Debug"

set "BUILD_PRESET=debug"
if /I "%CONFIG%"=="Release" set "BUILD_PRESET=release"

if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="Release" (
    echo [ERR] Unknown configuration: %CONFIG%
    exit /b 1
)

if not exist "%~dp0..\build\cmake-vs2026-x64" exit /b 0

pushd "%~dp0..\EzLaboratory" >nul

call "%~dp0CMake.bat" --build --preset %BUILD_PRESET% --target clean
set "RESULT=%ERRORLEVEL%"

popd >nul
exit /b %RESULT%
