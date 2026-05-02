# EzLaboratory 在另一台 Windows 电脑上的部署指南

本文档用于在一台新的 Windows 电脑上配置 EzLaboratory 项目开发环境，并使用 Xmake 生成 Visual Studio 工程。

> 当前项目技术栈：Windows + MSVC + Qt 6.8.3 msvc2022_64 + Xmake + Visual Studio。

---

## 1. 需要安装的软件

新电脑需要安装以下环境：

| 软件 | 用途 |
|---|---|
| Git | 拉取 GitHub 仓库 |
| Visual Studio 2022 | 提供 MSVC 编译器、MSBuild、Windows SDK |
| Qt 6.8.3 msvc2022_64 | 项目使用的 Qt SDK |
| Xmake | 读取 `xmake.lua` 并生成 VS 工程 |

推荐保持 Qt 版本和当前项目一致：

```text
Qt 6.8.3
msvc2022_64
```

---

## 2. 安装 Git

可以用 winget 安装：

```bat
winget install Git.Git
```

安装后重新打开 CMD，检查：

```bat
git --version
```

---

## 3. 安装 Visual Studio 2022

安装 Visual Studio 2022 Community / Professional 均可。

安装时必须勾选：

```text
使用 C++ 的桌面开发
Desktop development with C++
```

这个工作负载会安装：

```text
MSVC
MSBuild
Windows SDK
C++ 核心构建工具
```

安装完成后检查 MSVC 是否可用：

```bat
where cl
```

如果普通 CMD 找不到 `cl`，可以打开：

```text
x64 Native Tools Command Prompt for VS 2022
```

或者直接让 Xmake 自动查找 VS，一般不需要手动配置。

---

## 4. 安装 Qt 6.8.3

使用 Qt Online Installer 安装：

```text
Qt
  6.8.3
    msvc2022_64
```

安装后确认目录里存在：

```text
bin\qmake.exe
bin\moc.exe
bin\uic.exe
bin\rcc.exe
```

常见安装路径示例：

```text
C:\Qt\6.8.3\msvc2022_64
E:\Programme\Qt\6.8.3\msvc2022_64
```

建议配置 `QT_DIR` 环境变量。

永久配置：

```bat
setx QT_DIR "C:\Qt\6.8.3\msvc2022_64"
```

如果 Qt 安装在其他位置，替换成自己的路径，例如：

```bat
setx QT_DIR "E:\Programme\Qt\6.8.3\msvc2022_64"
```

执行 `setx` 后，需要重新打开 CMD 才会生效。

检查：

```bat
echo %QT_DIR%
dir "%QT_DIR%\bin\qmake.exe"
```

临时配置当前 CMD：

```bat
set QT_DIR=E:\Programme\Qt\6.8.3\msvc2022_64
```

---

## 5. 安装 Xmake

推荐使用 winget：

```bat
winget install xmake
```

安装后重新打开 CMD，检查：

```bat
xmake --version
```

如果 `winget` 不可用，可以使用 PowerShell 官方安装脚本：

```powershell
irm https://xmake.io/psget.text | iex
```

---

## 6. 拉取项目

进入自己的项目目录，例如：

```bat
cd /d F:\Project\C++
```

拉取仓库：

```bat
git clone https://github.com/Shulincooky/EzLaboratory.git
cd EzLaboratory
```

---

## 7. 生成 Visual Studio 工程

项目根目录应该包含：

```text
xmake.lua
build.bat
EzLaboratory\
```

执行：

```bat
build.bat
```

成功后会生成：

```text
vsxmake\EzLaboratory.sln
```

打开：

```text
vsxmake\EzLaboratory.sln
```

即可在 Visual Studio 中使用项目。

---

## 8. 核心 Xmake 命令说明

如果不使用 `build.bat`，也可以手动执行：

```bat
xmake f -y -p windows -a x64 -m debug --qt="%QT_DIR%"
xmake project -k vsxmake -m "debug,release"
```

其中：

| 命令 | 作用 |
|---|---|
| `xmake f` | 配置项目 |
| `-p windows` | 指定 Windows 平台 |
| `-a x64` | 指定 x64 架构 |
| `-m debug` | 使用 debug 模式 |
| `--qt="%QT_DIR%"` | 指定 Qt SDK 路径 |
| `xmake project -k vsxmake` | 生成 Visual Studio + Xmake 工程 |
| `-m "debug,release"` | 同时生成 debug / release 配置 |

---

## 9. 推荐的 build.bat

将下面的 `build.bat` 放在仓库根目录。

这个脚本只负责生成 Visual Studio 工程，不直接编译项目。

```bat
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
```

---

## 10. 常见问题

### 10.1 找不到 Xmake

现象：

```text
[ERR] xmake not found.
```

处理：

```bat
winget install xmake
```

安装后重新打开 CMD，再执行：

```bat
xmake --version
```

---

### 10.2 找不到 Qt SDK

现象：

```text
checking for Qt SDK ... no
```

处理：

1. 确认已经安装 `Qt 6.8.3 msvc2022_64`。
2. 设置 `QT_DIR`：

```bat
setx QT_DIR "C:\Qt\6.8.3\msvc2022_64"
```

3. 重新打开 CMD。
4. 再执行：

```bat
build.bat
```

---

### 10.3 VS 工程里的文件打不开

常见原因是手动移动了生成出来的 VS 工程目录，导致相对路径损坏。

处理：

```bat
rmdir /s /q vsxmake
build.bat
```

不要单独复制 `.sln` 或 `.vcxproj` 到别的目录。

---

### 10.4 编译时报中文编码错误

现象：

```text
warning C4819
error C2001: 常量中有换行符
```

处理：

确认 `xmake.lua` 里有：

```lua
set_encodings("utf-8")
```

建议写在 target 内部：

```lua
target("EzLaboratory")
    set_kind("binary")
    set_languages("cxx17")
    set_encodings("utf-8")
```

---

### 10.5 生成目录不是 vsxmake

Xmake 可能会根据 VS 版本生成：

```text
vsxmake2026
vsxmake2022
```

当前 `build.bat` 会自动把它重命名为：

```text
vsxmake
```

最终打开：

```text
vsxmake\EzLaboratory.sln
```

---

## 11. 最终部署检查清单

在新电脑上依次确认：

```bat
git --version
xmake --version
echo %QT_DIR%
dir "%QT_DIR%\bin\qmake.exe"
```

然后执行：

```bat
git clone https://github.com/Shulincooky/EzLaboratory.git
cd EzLaboratory
build.bat
```

看到下面内容即为成功：

```text
[OK] VS project generated:
...\EzLaboratory\vsxmake\EzLaboratory.sln
```

打开：

```text
vsxmake\EzLaboratory.sln
```

即可继续开发。

---

## 12. 参考资料

- Xmake Installation: https://xmake.io/#/guide/installation
- Xmake Project Generator: https://xmake.io/#/plugin/builtin_plugins?id=generate-ide-project-files
- Qt 6.8 for Windows: https://doc.qt.io/qt-6.8/windows.html
- Qt 6.8 Supported Platforms: https://doc.qt.io/qt-6.8/supported-platforms.html
- Visual Studio C++ Workload: https://learn.microsoft.com/visualstudio/install/workload-component-id-vs-build-tools
