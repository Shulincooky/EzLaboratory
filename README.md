# EzLaboratory

EzLaboratory uses CMake as the only build system. The Visual Studio entry point is a thin `.slnx` wrapper so that Solution Explorer can use "Show All Files" with the real source directory.

## Layout

```text
EzLaboratory/
  build.bat
  EzLaboratory/
    EzLaboratory.slnx
    EzLaboratory.vcxproj
    CMakeLists.txt
    CMakePresets.json
    source/
    resources/
  scripts/
  vendor/
    cmake-4.3.2-windows-x86_64/
  build/
```

## Visual Studio

Open:

```text
EzLaboratory/EzLaboratory.slnx
```

In Solution Explorer, enable "Show All Files". The project root is the physical source directory:

```text
EzLaboratory/EzLaboratory
```

Build, rebuild, clean, and F5 are forwarded to CMake through the Makefile `.vcxproj`.

## Command Line

Debug build:

```bat
build.bat
```

Release build:

```bat
build.bat release
```

Equivalent CMake commands:

```bat
cd EzLaboratory
..\scripts\CMake.bat --preset vs2026-x64
..\scripts\CMake.bat --build --preset debug
```

`scripts/CMake.bat` first uses the portable CMake in `vendor/cmake-4.3.2-windows-x86_64/bin/cmake.exe`, then falls back to `cmake` on `PATH`.

## Requirements

- Visual Studio 2026 with C++ desktop tools
- Qt 6 for MSVC x64
- Portable CMake already included under `vendor`

Qt is found in this order:

1. `CMAKE_PREFIX_PATH` if you pass it yourself
2. `QT_DIR` environment variable
3. Known local install paths in `CMakeLists.txt`

Example:

```bat
set QT_DIR=E:\Programme\Qt\6.8.3\msvc2022_64
build.bat
```

Generated files stay under `build/` and are ignored by git.
