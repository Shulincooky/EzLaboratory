cmake_minimum_required(VERSION 3.27)

project(EzLaboratory VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

foreach(qt_env_var IN ITEMS QT_DIR QTDIR)
    if(DEFINED ENV{${qt_env_var}} AND NOT "$ENV{${qt_env_var}}" STREQUAL "")
        list(PREPEND CMAKE_PREFIX_PATH "$ENV{${qt_env_var}}")
    endif()
endforeach()

find_package(Qt6 QUIET COMPONENTS Widgets Svg Network)
if(NOT Qt6_FOUND)
    message(FATAL_ERROR
        "Qt6 was not found. Set QT_DIR or QTDIR to your Qt kit root "
        "(for example E:/Programme/Qt/6.8.3/msvc2022_64), or pass "
        "-DCMAKE_PREFIX_PATH=<Qt kit root>."
    )
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/EzLaboratory")
set(SOURCE_DIR "${APP_DIR}/source")
set(RESOURCE_DIR "${APP_DIR}/resources")

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.hpp"
    "${RESOURCE_DIR}/*.qrc"
    "${RESOURCE_DIR}/*.rc"
)

file(GLOB_RECURSE APP_UI_FILES CONFIGURE_DEPENDS
    "${SOURCE_DIR}/*.ui"
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
    ${APP_UI_FILES}
    ${APP_ASSETS}
)

set(APP_GENERATED_UI_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/ui")
file(MAKE_DIRECTORY "${APP_GENERATED_UI_DIR}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${APP_UI_FILES})

get_target_property(QT_UIC_EXECUTABLE Qt6::uic IMPORTED_LOCATION)
if(NOT QT_UIC_EXECUTABLE)
    message(FATAL_ERROR "Qt uic executable was not found.")
endif()

foreach(ui_file IN LISTS APP_UI_FILES)
    get_filename_component(ui_base_name "${ui_file}" NAME_WE)
    if(DEFINED APP_UI_HEADER_${ui_base_name})
        message(FATAL_ERROR "Duplicate Qt Designer form name: ${ui_base_name}.ui")
    endif()
    set(APP_UI_HEADER_${ui_base_name} TRUE)
    set(ui_header "${APP_GENERATED_UI_DIR}/ui_${ui_base_name}.h")
    execute_process(
        COMMAND "${QT_UIC_EXECUTABLE}" -o "${ui_header}" "${ui_file}"
        COMMAND_ERROR_IS_FATAL ANY
    )
endforeach()

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
target_include_directories(EzLaboratory PRIVATE "${APP_GENERATED_UI_DIR}")

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
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
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
