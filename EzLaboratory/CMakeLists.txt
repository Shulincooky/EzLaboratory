cmake_minimum_required(VERSION 3.27)

project(EzLaboratory VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

set(EZ_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(EZ_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/resources")
set(EZ_OUTPUT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../build")

if(NOT CMAKE_PREFIX_PATH)
    if(DEFINED ENV{QT_DIR} AND EXISTS "$ENV{QT_DIR}/lib/cmake/Qt6/Qt6Config.cmake")
        list(APPEND CMAKE_PREFIX_PATH "$ENV{QT_DIR}")
    endif()

    foreach(qt_candidate
        "E:/Programme/Qt/6.8.3/msvc2022_64"
        "C:/Qt/6.10.0/msvc2022_64"
        "C:/Qt/6.9.0/msvc2022_64"
        "C:/Qt/6.8.3/msvc2022_64"
    )
        if(NOT CMAKE_PREFIX_PATH AND EXISTS "${qt_candidate}/lib/cmake/Qt6/Qt6Config.cmake")
            list(APPEND CMAKE_PREFIX_PATH "${qt_candidate}")
            break()
        endif()
    endforeach()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE EZ_SOURCES CONFIGURE_DEPENDS
    "${EZ_SOURCE_DIR}/*.cpp"
    "${EZ_SOURCE_DIR}/*.h"
    "${EZ_SOURCE_DIR}/*.hpp"
    "${EZ_SOURCE_DIR}/*.ui"
)

file(GLOB_RECURSE EZ_QRC_FILES CONFIGURE_DEPENDS
    "${EZ_RESOURCE_DIR}/*.qrc"
)

file(GLOB_RECURSE EZ_RC_FILES CONFIGURE_DEPENDS
    "${EZ_RESOURCE_DIR}/*.rc"
)

file(GLOB_RECURSE EZ_ASSET_FILES CONFIGURE_DEPENDS
    "${EZ_RESOURCE_DIR}/*.ico"
    "${EZ_RESOURCE_DIR}/*.svg"
    "${EZ_RESOURCE_DIR}/*.json"
    "${EZ_RESOURCE_DIR}/*.png"
)

qt_add_executable(EzLaboratory
    WIN32
    ${EZ_SOURCES}
    ${EZ_QRC_FILES}
    ${EZ_RC_FILES}
    ${EZ_ASSET_FILES}
)

set_source_files_properties(${EZ_ASSET_FILES} PROPERTIES HEADER_FILE_ONLY ON)

set(EZ_INCLUDE_DIRS "${EZ_SOURCE_DIR}")
foreach(source_file IN LISTS EZ_SOURCES)
    get_filename_component(source_dir "${source_file}" DIRECTORY)
    list(APPEND EZ_INCLUDE_DIRS "${source_dir}")
endforeach()
list(REMOVE_DUPLICATES EZ_INCLUDE_DIRS)

target_include_directories(EzLaboratory PRIVATE
    ${EZ_INCLUDE_DIRS}
)

target_compile_definitions(EzLaboratory PRIVATE
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

target_compile_options(EzLaboratory PRIVATE
    /utf-8
    /EHsc
    /Zc:__cplusplus
    /permissive-
)

target_link_libraries(EzLaboratory PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
)

set_target_properties(EzLaboratory PROPERTIES
    AUTOUIC_SEARCH_PATHS "${EZ_INCLUDE_DIRS}"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:EzLaboratory>"
)

foreach(output_config Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${output_config}" output_config_upper)
    set_target_properties(EzLaboratory PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${output_config_upper} "${EZ_OUTPUT_ROOT}/bin/${output_config}"
        LIBRARY_OUTPUT_DIRECTORY_${output_config_upper} "${EZ_OUTPUT_ROOT}/lib/${output_config}"
        ARCHIVE_OUTPUT_DIRECTORY_${output_config_upper} "${EZ_OUTPUT_ROOT}/lib/${output_config}"
    )
endforeach()

add_custom_command(TARGET EzLaboratory POST_BUILD
    COMMAND Qt6::windeployqt
        --no-translations
        --no-system-d3d-compiler
        --no-opengl-sw
        --no-compiler-runtime
        "$<TARGET_FILE:EzLaboratory>"
    VERBATIM
)
