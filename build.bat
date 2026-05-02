@echo off
setlocal
cd /d "%~dp0"

where xmake >nul 2>nul || (
    echo [ERR] xmake not found.
    exit /b 1
)

if not defined QT_DIR (
    for %%Q in (
        "E:\Programme\Qt\6.8.3\msvc2022_64"
        "C:\Qt\6.10.0\msvc2022_64"
        "C:\Qt\6.9.0\msvc2022_64"
        "C:\Qt\6.8.3\msvc2022_64"
    ) do if not defined QT_DIR if exist "%%~Q\bin\qmake.exe" set "QT_DIR=%%~Q"
)

if defined QT_DIR (
    xmake f -y -p windows -a x64 -m debug --qt="%QT_DIR%"
) else (
    xmake f -y -p windows -a x64 -m debug
)
if errorlevel 1 exit /b 1

for /d %%D in (vsxmake*) do rmdir /s /q "%%D" 2>nul

xmake project -k vsxmake -m "debug,release"
if errorlevel 1 exit /b 1

for /d %%D in (vsxmake*) do if exist "%%D\EzLaboratory.sln" (
    echo.
    echo [OK] VS project generated:
    echo %cd%\%%D\EzLaboratory.sln
    exit /b 0
)

echo [ERR] VS project generation failed. No .sln found.
exit /b 1