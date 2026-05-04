add_rules("mode.debug", "mode.release")

set_project("EzLaboratory")
set_version("0.1.0")
set_defaultmode("debug")

target("EzLaboratory")
    set_kind("binary")
    set_languages("cxx17")
    set_encodings("utf-8")

    add_rules("qt.widgetapp")

    -- 当前已有模块。
    -- 如果之后前端接 HTTP/TCP，再加 QtNetwork。
    add_frameworks("QtCore", "QtGui", "QtWidgets", "QtSvg")

    -- 物理目录 include。
    -- 这样 app/main.cpp 里可以继续 #include "EzLaboratory.h"，
    -- laboratory 下的类也可以继续互相 include。
    add_includedirs(
        "EzLaboratory/app",
        "EzLaboratory/laboratory",
        "EzLaboratory/tool"
    )

    -- 根据物理路径自动加入源码。
    add_files("EzLaboratory/**.cpp")

    -- 自动加入 Qt Designer UI 文件。
    add_files("EzLaboratory/**.ui")

    -- 自动加入 Qt 资源文件。
    add_files("EzLaboratory/**.qrc")

    -- 自动加入头文件，让含 Q_OBJECT 的头参与 Qt moc。
    -- 这可以替代你原来手动列 AbstractBottleItem.h / BeakerItem.h / ... 的方式。
    add_files("EzLaboratory/**.h")

    -- 让头文件出现在 VS / VSXMake 工程里。
    add_headerfiles("EzLaboratory/**.h", { install = false })

    -- 资源文件只作为工程展示，不参与编译。
    -- 真正参与编译的是 .qrc。
    add_extrafiles(
        "EzLaboratory/resources/**",
        "resources/**",
        { install = false }
    )

    -- VS 工程中按物理路径显示。
    add_filegroups("Source", {
        rootdir = "EzLaboratory",
        mode = "plain",
        files = {
            "app/**",
            "laboratory/**",
            "tool/**"
        }
    })

    add_filegroups("Resources", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/resources/**",
            "resources/**"
        }
    })

    if is_mode("debug") then
        set_runtimes("MDd")
    else
        set_runtimes("MD")
    end