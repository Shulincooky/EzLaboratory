@echo off
setlocal

set "SAVED_PATH=%PATH%"
set "PATH="
set "Path="
set "PATH=%SAVED_PATH%"

set "ROOT_DIR=%~dp0.."
set "CMAKE_EXE=%ROOT_DIR%\vendor\cmake-4.3.2-windows-x86_64\bin\cmake.exe"

if not exist "%CMAKE_EXE%" (
    where cmake >nul 2>nul
    if errorlevel 1 (
        echo [ERR] CMake not found. Expected "%CMAKE_EXE%" or cmake on PATH.
        exit /b 1
    )
    set "CMAKE_EXE=cmake"
)

"%CMAKE_EXE%" %*
exit /b %ERRORLEVEL%
