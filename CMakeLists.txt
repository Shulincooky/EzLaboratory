cmake_minimum_required(VERSION 3.27)

project(EzLaboratory VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

if(DEFINED ENV{QT_DIR})
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{QT_DIR}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Network)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EzLaboratory")
set(SOURCE_DIR "${APP_DIR}/source")
set(RESOURCE_DIR "${APP_DIR}/resources")

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.hpp"
    "${SOURCE_DIR}/*.ui"
    "${RESOURCE_DIR}/*.qrc"
    "${RESOURCE_DIR}/*.rc"
)

file(GLOB_RECURSE APP_ASSETS CONFIGURE_DEPENDS
    "${RESOURCE_DIR}/*.ico"
    "${RESOURCE_DIR}/*.json"
    "${RESOURCE_DIR}/*.png"
    "${RESOURCE_DIR}/*.svg"
)

qt_add_executable(EzLaboratory
    WIN32
    ${APP_SOURCES}
    ${APP_ASSETS}
)

set_source_files_properties(${APP_ASSETS} PROPERTIES HEADER_FILE_ONLY ON)

set(APP_INCLUDE_DIRS "${SOURCE_DIR}")
foreach(source_file IN LISTS APP_SOURCES)
    get_filename_component(source_dir "${source_file}" DIRECTORY)
    string(FIND "${source_dir}" "${SOURCE_DIR}" source_dir_pos)
    if(source_dir_pos EQUAL 0)
        list(APPEND APP_INCLUDE_DIRS "${source_dir}")
    endif()
endforeach()
list(REMOVE_DUPLICATES APP_INCLUDE_DIRS)

target_include_directories(EzLaboratory PRIVATE ${APP_INCLUDE_DIRS})

target_compile_definitions(EzLaboratory PRIVATE
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
)

target_compile_options(EzLaboratory PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->
)

target_link_libraries(EzLaboratory PRIVATE
    Qt6::Widgets
    Qt6::Svg
    Qt6::Network
)

set_target_properties(EzLaboratory PROPERTIES
    AUTOUIC_SEARCH_PATHS "${APP_INCLUDE_DIRS}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:EzLaboratory>"
)

if(WIN32 AND TARGET Qt6::windeployqt)
    add_custom_command(TARGET EzLaboratory POST_BUILD
        COMMAND Qt6::windeployqt
            --no-translations
            --no-system-d3d-compiler
            --no-opengl-sw
            --no-compiler-runtime
            "$<TARGET_FILE:EzLaboratory>"
        VERBATIM
    )
endif()
