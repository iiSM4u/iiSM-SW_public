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

# Check if the DLL files exist
if(NOT EXISTS ${MIICAM_DLL_PATH})
    message(FATAL_ERROR "DLL file not found: ${MIICAM_DLL_PATH}")
endif()

# Check if the LIB file exists
if(NOT EXISTS ${MIICAM_LIB_PATH})
    message(FATAL_ERROR "LIB file not found: ${MIICAM_LIB_PATH}")
endif()

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

# header 파일들 - C/C++ -> 일반 -> 추가포함 디렉터리와 같다.
include_directories(
    ${GEGL_INCLUDE_DIRS}
    ${BABL_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GOBJECT_INCLUDE_DIRS}
    ${GETTEXT_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/external
)

# lib 파일이 존재하는 directory - linker -> 일반 -> 추가 라이브러리 디렉터리와 같다.
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

# 실제 참조할 lib 파일들 - linker -> 입력 -> 추가 종속성과 같다.
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

# Add a post-build step to copy the DLL to the output directory
add_custom_command(TARGET SIMP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MIICAM_DLL_PATH} $<TARGET_FILE_DIR:SIMP>
)

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
