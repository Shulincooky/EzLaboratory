add_rules("mode.debug", "mode.release")

set_project("EzLaboratory")
set_version("0.1.0")
set_defaultmode("debug")

target("EzLaboratory")
    set_kind("binary")
    set_languages("cxx17")
    set_encodings("utf-8")

    add_rules("qt.widgetapp")
    add_frameworks("QtCore", "QtGui", "QtWidgets", "QtSvg")

    add_includedirs("EzLaboratory")

    add_files("EzLaboratory/*.cpp")
    add_files("EzLaboratory/*.ui")
    add_files("EzLaboratory/*.qrc")

    -- 让头文件出现在 VS / VSXMake 工程里，并参与 add_filegroups 分组。
    -- install = false 表示这些头文件只是工程展示/开发使用，不作为安装文件处理。
    add_headerfiles("EzLaboratory/*.h", {install = false})

    -- 不再把 resources/** 作为 extrafiles 整棵挂进 VS 工程。
    -- qrc 资源编译只需要 add_files("EzLaboratory/*.qrc")。
    -- add_extrafiles("EzLaboratory/resources/**") 会让 VS 生成器按物理目录额外挂一棵 EzLaboratory/resources。

    -- Qt moc headers
    add_files(
        "EzLaboratory/AbstractBottleItem.h",
        "EzLaboratory/AbstractLiquidContainerItem.h",
        "EzLaboratory/AbstractPlugItem.h",
        "EzLaboratory/BeakerItem.h",
        "EzLaboratory/BeakerPourHandleItem.h",
        "EzLaboratory/EzLaboratory.h",
        "EzLaboratory/LabGraphicsView.h",
        "EzLaboratory/LabItem.h",
        "EzLaboratory/LabwareItemDelegate.h",
        "EzLaboratory/LabwareListView.h",
        "EzLaboratory/LiquidItem.h",
        "EzLaboratory/MainWindow.h",
        "EzLaboratory/NarrowBottleItem.h",
        "EzLaboratory/NarrowBottlePlugItem.h",
        "EzLaboratory/TweezersItem.h",
        "EzLaboratory/WideBottleItem.h",
        "EzLaboratory/WideBottlePlugItem.h"
    )

    local function group(name, files)
        add_filegroups(name, {
            rootdir = "EzLaboratory",
            mode = "plain",
            files = files
        })
    end

    group("Source", {
        "main.cpp",
        "EzLaboratory.cpp",
        "EzLaboratory.h",
        "MainWindow.cpp",
        "MainWindow.h"
    })

    group("Source/Scene", {
        "LabGraphicsView.*",
        "LabItem.*",
        "LabwareItemDelegate.*",
        "LabwareListView.*",
        "LabwareTemplateRoles.h"
    })

    group("Source/Labware/Bottles", {
        "AbstractBottleItem.*",
        "AbstractPlugItem.*",
        "NarrowBottleItem.*",
        "NarrowBottlePlugItem.*",
        "WideBottleItem.*",
        "WideBottlePlugItem.*",
        "BottleLabel*.*"
    })

    group("Source/Labware/Containers", {
        "AbstractLiquidContainerItem.*",
        "BeakerItem.*",
        "BeakerPourHandleItem.*",
        "LiquidItem.*"
    })

    group("Source/Labware/Tools", {
        "SolidScatterItem.*",
        "TweezersItem.*"
    })

    group("Source/Config", {
        "ExperimentSidebarConfigLoader.*"
    })

    group("Source/Qt", {
        "EzLaboratory.ui",
        "MainWindow.ui",
        "EzLaboratory.qrc"
    })

    if is_mode("debug") then
        set_runtimes("MDd")
    else
        set_runtimes("MD")
    end