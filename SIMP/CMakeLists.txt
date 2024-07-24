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

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Multimedia MultimediaWidgets Widgets)

# 실행을 위한 파일들
file(GLOB_RECURSE HEADER_FILES "include/*.h")
file(GLOB_RECURSE SOURCE_FILES "source/*.cpp")
file(GLOB_RECURSE UI_FILES "ui/*.ui")

# Project source files
set(PROJECT_SOURCES
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    mainwindow.cpp  # 이유는 모르겠는데 mainwindow.cpp가 source 폴더 아래에 있으면 빌드가 안된다.
)

# Set OpenCV_DIR to the directory containing OpenCVConfig.cmake
set(OpenCV_DIR "C:/opencv_mingw/")
find_package(OpenCV REQUIRED)

# Find GEGL and BABL using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GEGL REQUIRED gegl-0.4)
pkg_check_modules(BABL REQUIRED babl-0.1)
pkg_check_modules(GLIB REQUIRED glib-2.0)

# Include directories
include_directories(
    ${GEGL_INCLUDE_DIRS}
    ${BABL_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/include  # custom ui의 header를 참조할 수 있도록 추가
)

# Link directories
link_directories(
    ${GEGL_LIBRARY_DIRS}
    ${BABL_LIBRARY_DIRS}
    ${GLIB_LIBRARY_DIRS}
    ${CMAKE_SOURCE_DIR}/lib
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

# Find all libraries in the lib directory
file(GLOB SIMP_LIBS
    "${CMAKE_SOURCE_DIR}/lib/*.lib"
    "${CMAKE_SOURCE_DIR}/lib/*.dll.a"
)

# Link libraries
target_link_libraries(SIMP PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    ${GEGL_LIBRARIES}
    ${BABL_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${OpenCV_LIBS}
    ${SIMP_LIBS}
)

# Find all DLLs in the bin directory
file(GLOB SIMP_DLLS "${CMAKE_SOURCE_DIR}/bin/*.dll")

foreach(dll ${SIMP_DLLS})
    add_custom_command(TARGET SIMP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${dll}
        $<TARGET_FILE_DIR:SIMP>
    )
endforeach()

file(GLOB OpenCV_DLLS "${OpenCV_DIR}/x64/mingw/bin/*.dll")

foreach(dll ${OpenCV_DLLS})
    add_custom_command(TARGET SIMP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${dll}
        $<TARGET_FILE_DIR:SIMP>
    )
endforeach()

set(GEGL_BIN_DIRS "${GEGL_LIBRARY_DIRS}/../bin")

set(GEGL_DLLS
    "${GEGL_BIN_DIRS}/libgegl-0.4-0.dll"
    "${GEGL_BIN_DIRS}/libbabl-0.1-0.dll"
    "${GEGL_BIN_DIRS}/libglib-2.0-0.dll"
    "${GEGL_BIN_DIRS}/libgobject-2.0-0.dll"
    "${GEGL_BIN_DIRS}/libgmodule-2.0-0.dll"
    "${GEGL_BIN_DIRS}/libgthread-2.0-0.dll"
    "${GEGL_BIN_DIRS}/libgio-2.0-0.dll"
    "${GEGL_BIN_DIRS}/libintl-8.dll"
)

foreach(dll ${GEGL_DLLS})
    add_custom_command(TARGET SIMP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${dll}
        $<TARGET_FILE_DIR:SIMP>
    )
endforeach()

# Copy GEGL plugin DLLs to the target directory
file(GLOB GEGL_PLUGIN_DLLS "${GEGL_LIBRARY_DIRS}/gegl-0.4/*.dll")

foreach(PLUGIN ${GEGL_PLUGIN_DLLS})
    add_custom_command(TARGET SIMP POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:SIMP>/lib/gegl-0.4
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PLUGIN}
        $<TARGET_FILE_DIR:SIMP>/lib/gegl-0.4/
    )
endforeach()

# Copy BABL plugin DLLs to the target directory
file(GLOB BABL_PLUGIN_DLLS "${BABL_LIBRARY_DIRS}/babl-0.1/*.dll")

foreach(PLUGIN ${BABL_PLUGIN_DLLS})
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
