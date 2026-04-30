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

    add_headerfiles("EzLaboratory/*.h", {install = false})
    add_extrafiles("EzLaboratory/resources/**")

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

    add_filegroups("Source Files/Core", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/main.cpp",
            "EzLaboratory/EzLaboratory.cpp",
            "EzLaboratory/MainWindow.cpp"
        }
    })

    add_filegroups("Source Files/Scene", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/LabGraphicsView.cpp",
            "EzLaboratory/LabItem.cpp",
            "EzLaboratory/LabwareItemDelegate.cpp",
            "EzLaboratory/LabwareListView.cpp"
        }
    })

    add_filegroups("Source Files/Labware", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/AbstractBottleItem.cpp",
            "EzLaboratory/AbstractLiquidContainerItem.cpp",
            "EzLaboratory/AbstractPlugItem.cpp",
            "EzLaboratory/BeakerItem.cpp",
            "EzLaboratory/BeakerPourHandleItem.cpp",
            "EzLaboratory/BottleLabelItem.cpp",
            "EzLaboratory/BottleLabelRenderer.cpp",
            "EzLaboratory/LiquidItem.cpp",
            "EzLaboratory/NarrowBottleItem.cpp",
            "EzLaboratory/NarrowBottlePlugItem.cpp",
            "EzLaboratory/SolidScatterItem.cpp",
            "EzLaboratory/TweezersItem.cpp",
            "EzLaboratory/WideBottleItem.cpp",
            "EzLaboratory/WideBottlePlugItem.cpp"
        }
    })

    add_filegroups("Source Files/Config", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/ExperimentSidebarConfigLoader.cpp"
        }
    })

    add_filegroups("Header Files/Core", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/EzLaboratory.h",
            "EzLaboratory/MainWindow.h"
        }
    })

    add_filegroups("Header Files/Scene", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/LabGraphicsView.h",
            "EzLaboratory/LabItem.h",
            "EzLaboratory/LabwareItemDelegate.h",
            "EzLaboratory/LabwareListView.h",
            "EzLaboratory/LabwareTemplateRoles.h"
        }
    })

    add_filegroups("Header Files/Labware", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/AbstractBottleItem.h",
            "EzLaboratory/AbstractLiquidContainerItem.h",
            "EzLaboratory/AbstractPlugItem.h",
            "EzLaboratory/BeakerItem.h",
            "EzLaboratory/BeakerPourHandleItem.h",
            "EzLaboratory/BottleLabelData.h",
            "EzLaboratory/BottleLabelItem.h",
            "EzLaboratory/BottleLabelRenderer.h",
            "EzLaboratory/LiquidItem.h",
            "EzLaboratory/NarrowBottleItem.h",
            "EzLaboratory/NarrowBottlePlugItem.h",
            "EzLaboratory/SolidScatterItem.h",
            "EzLaboratory/TweezersItem.h",
            "EzLaboratory/WideBottleItem.h",
            "EzLaboratory/WideBottlePlugItem.h"
        }
    })

    add_filegroups("Header Files/Config", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/ExperimentSidebarConfigLoader.h"
        }
    })

    add_filegroups("Forms", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/EzLaboratory.ui",
            "EzLaboratory/MainWindow.ui"
        }
    })

    add_filegroups("Resources/QRC", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/EzLaboratory.qrc"
        }
    })

    add_filegroups("Resources/Images", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/resources/image/**"
        }
    })

    add_filegroups("Resources/Config", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/resources/config/**"
        }
    })

    add_filegroups("Resources/Icons", {
        rootdir = ".",
        mode = "plain",
        files = {
            "EzLaboratory/resources/icon/**",
            "EzLaboratory/resources/*.svg"
        }
    })

    if is_mode("debug") then
        set_runtimes("MDd")
    else
        set_runtimes("MD")
    end