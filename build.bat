@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

where git >nul 2>nul && (
    if exist ".git" git pull --rebase --autostash
)

where xmake >nul 2>nul || (
    echo [ERR] xmake not found.
    exit /b 1
)

if not defined QT_DIR (
    for %%Q in (
        "E:\Programme\Qt\6.8.3\msvc2022_64"
        "C:\Qt\6.8.3\msvc2022_64"
        "C:\Qt\6.9.0\msvc2022_64"
        "C:\Qt\6.10.0\msvc2022_64"
    ) do (
        if exist "%%~Q\bin\qmake.exe" set "QT_DIR=%%~Q"
    )
)

if defined QT_DIR (
    xmake f -y -p windows -a x64 -m debug --qt="!QT_DIR!"
) else (
    xmake f -y -p windows -a x64 -m debug
)

if errorlevel 1 exit /b 1

rmdir /s /q vsxmake 2>nul
rmdir /s /q vsxmake2026 2>nul
rmdir /s /q vsxmake2025 2>nul
rmdir /s /q vsxmake2022 2>nul

xmake project -k vsxmake -m "debug,release"
if errorlevel 1 exit /b 1

set "GEN="
for /d %%D in (vsxmake*) do (
    if exist "%%D\EzLaboratory.sln" set "GEN=%%D"
)

if not defined GEN (
    echo [ERR] VS project generation failed. No generated solution found.
    exit /b 1
)

ren "%GEN%" vsxmake

set "SLN=vsxmake\EzLaboratory.sln"
if not exist "%SLN%" (
    echo [ERR] VS project generation failed. No .sln found.
    exit /b 1
)

echo.
echo [OK] VS project generated:
echo %cd%\%SLN%

exit /b 0