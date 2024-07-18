# CMake minimum version required
cmake_minimum_required(VERSION 3.5)

# Project name and version
project(SIMP VERSION 0.1 LANGUAGES CXX)

# Automatically handle UI, MOC, and RCC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define paths for DLL and LIB files
set(GIMP_PATH "C:/Program Files/GIMP 2")
set(GEGL_DLL_PATHS
    "${GIMP_PATH}/bin/libbabl-0.1-0.dll"
    "${GIMP_PATH}/bin/libgegl-0.4-0.dll"
    "${GIMP_PATH}/bin/libgio-2.0-0.dll"
    "${GIMP_PATH}/bin/libglib-2.0-0.dll"
    "${GIMP_PATH}/bin/libgmodule-2.0-0.dll"
    "${GIMP_PATH}/bin/libgobject-2.0-0.dll"
    "${GIMP_PATH}/bin/libgthread-2.0-0.dll"
    "${GIMP_PATH}/bin/libintl-8.dll"
)

set(GEGL_PLUGIN_DIR "${GIMP_PATH}/lib/gegl-0.4")
set(BABL_PLUGIN_DIR "${GIMP_PATH}/lib/babl-0.1")

set(MIICAM_DLL_PATH "${CMAKE_SOURCE_DIR}/bin/miicam.dll")
set(MIICAM_LIB_PATH "${CMAKE_SOURCE_DIR}/lib/miicam.lib")

# Project source files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    PixelFormatType.h
    constants.h
)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Multimedia MultimediaWidgets Widgets)

# Find GEGL and BABL using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GEGL REQUIRED gegl-0.4)
pkg_check_modules(BABL REQUIRED babl-0.1)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)

# Manually specify the gettext paths
set(GETTEXT_INCLUDE_DIRS "C:/msys64/mingw64/include")
set(GETTEXT_LIBRARY_DIRS "C:/msys64/mingw64/lib")
set(GETTEXT_LIBRARIES "C:/msys64/mingw64/lib/libintl.a")

# Include directories
include_directories(
    ${GEGL_INCLUDE_DIRS}
    ${BABL_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GOBJECT_INCLUDE_DIRS}
    ${GETTEXT_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/external
)

# Link directories
link_directories(
    ${GEGL_LIBRARY_DIRS}
    ${BABL_LIBRARY_DIRS}
    ${GLIB_LIBRARY_DIRS}
    ${GOBJECT_LIBRARY_DIRS}
    ${GETTEXT_LIBRARY_DIRS}
)

# Add executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SIMP
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(SIMP SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(SIMP
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link libraries
target_link_libraries(SIMP PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    ${GEGL_LIBRARIES}
    ${BABL_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GETTEXT_LIBRARIES}
    ${MIICAM_LIB_PATH}
)

# Add a post-build step to copy the DLLs to the output directory
foreach(DLL ${GEGL_DLL_PATHS})
    add_custom_command(TARGET SIMP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL}
        $<TARGET_FILE_DIR:SIMP>
    )
endforeach()

add_custom_command(TARGET SIMP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${MIICAM_DLL_PATH}
    $<TARGET_FILE_DIR:SIMP>
)

# Add a post-build step to copy GEGL and BABL plugin files to the output directory
file(GLOB GEGL_PLUGINS "${GEGL_PLUGIN_DIR}/*.dll")
file(GLOB BABL_PLUGINS "${BABL_PLUGIN_DIR}/*.dll")

foreach(PLUGIN ${GEGL_PLUGINS})
    add_custom_command(TARGET SIMP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SIMP>/lib/gegl-0.4
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PLUGIN}
        $<TARGET_FILE_DIR:SIMP>/lib/gegl-0.4/
    )
endforeach()

foreach(PLUGIN ${BABL_PLUGINS})
    add_custom_command(TARGET SIMP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SIMP>/lib/babl-0.1
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PLUGIN}
        $<TARGET_FILE_DIR:SIMP>/lib/babl-0.1/
    )
endforeach()

# Set target properties
if(${QT_VERSION_MAJOR} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SIMP)
endif()
set_target_properties(SIMP PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation settings
include(GNUInstallDirs)
install(TARGETS SIMP
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SIMP)
endif()
