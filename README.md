# EzLaboratory

EzLaboratory is a standard Qt/CMake project.

## Open In Visual Studio

Use **File > Open > Folder...** and open the repository root:

```text
F:\Project\C++\EzLaboratory
```

Visual Studio reads `CMakeLists.txt` and `CMakePresets.json` directly. Do not open a generated `.sln` or `.vcxproj`.

## Build

From the repository root:

```bat
cmake --preset vs2026-x64
cmake --build --preset debug
```

Release:

```bat
cmake --build --preset release
```

Generated files stay under `build/`.

## Qt

Qt is found in this order:

1. `CMAKE_PREFIX_PATH`
2. `QT_DIR`

Example:

```bat
set QT_DIR=E:\Programme\Qt\6.8.3\msvc2022_64
cmake --preset vs2026-x64
cmake --build --preset debug
```

To add a Qt module, add it once in `find_package` and `target_link_libraries`.
