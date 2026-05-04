add_rules("mode.debug", "mode.release")

set_project("EzLaboratory")
set_version("0.1.0")
set_defaultmode("debug")

target("EzLaboratory")
    set_kind("binary")
    set_languages("cxx17")
    set_encodings("utf-8")

    add_rules("qt.widgetapp")

    add_frameworks(
        "QtCore",
        "QtGui",
        "QtWidgets",
        "QtSvg"
    )

    -- =========================
    -- 目录变量
    -- =========================
    local project_dir = "EzLaboratory"
    local source_dir = path.join(project_dir, "source")
    local resource_dir = path.join(project_dir, "resources")

    -- =========================
    -- Include 路径
    -- =========================
    add_includedirs(
        source_dir,
        path.join(source_dir, "app"),
        path.join(source_dir, "frontend"),
        path.join(source_dir, "laboratory"),
        path.join(source_dir, "network"),
        path.join(source_dir, "payload"),
        path.join(source_dir, "tool")
    )

    -- =========================
    -- 编译文件
    -- =========================

    -- C++ 源文件
    add_files(path.join(source_dir, "**.cpp"))

    -- Qt UI 文件
    add_files(path.join(source_dir, "**.ui"))

    -- Qt Resource 文件
    -- 注意：qrc 只允许通过 add_files 加一次
    add_files(path.join(resource_dir, "**.qrc"))

    -- Qt MOC 头文件
    add_files(path.join(source_dir, "**.h"))

    -- 让头文件在 VS 工程中显示
    add_headerfiles(path.join(source_dir, "**.h"), { install = false })

    -- =========================
    -- 资源文件展示
    -- =========================
    -- 不要写 resources/**，否则 qrc 会被重复加入
    add_extrafiles(
        path.join(resource_dir, "config/**"),
        path.join(resource_dir, "image/**"),
        { install = false }
    )

    -- =========================
    -- VS 工程筛选器分组
    -- =========================

    add_filegroups("Source", {
        rootdir = source_dir,
        mode = "plain",
        files = {
            "app/**",
            "frontend/**",
            "laboratory/**",
            "network/**",
            "payload/**",
            "tool/**"
        }
    })

    add_filegroups("Resources", {
        rootdir = resource_dir,
        mode = "plain",
        files = {
            "EzLaboratory.qrc",
            "config/**",
            "image/**"
        }
    })

    if is_mode("debug") then
        set_runtimes("MDd")
    else
        set_runtimes("MD")
    end