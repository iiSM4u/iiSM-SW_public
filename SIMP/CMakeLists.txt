
cmake_minimum_required(VERSION 3.5)

project(SIMP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MIICAM_DLL_PATH "${CMAKE_SOURCE_DIR}/bin/miicam.dll")
set(MIICAM_LIB_PATH "${CMAKE_SOURCE_DIR}/lib/miicam.lib")

# Check if the DLL file exists
if(NOT EXISTS ${MIICAM_DLL_PATH})
    message(FATAL_ERROR "DLL file not found: ${MIICAM_DLL_PATH}")
endif()

# Check if the LIB file exists
if(NOT EXISTS ${MIICAM_LIB_PATH})
    message(FATAL_ERROR "LIB file not found: ${MIICAM_LIB_PATH}")
endif()


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Multimedia MultimediaWidgets Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    PixelFormatType.h
    constants.h
)

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

# Include the directory where miicam.h is located
include_directories(${CMAKE_SOURCE_DIR}/external)

# Link the miicam library
target_link_libraries(SIMP PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    ${MIICAM_LIB_PATH}
)

# Add a post-build step to copy the DLL to the output directory
add_custom_command(TARGET SIMP POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${MIICAM_DLL_PATH}
    $<TARGET_FILE_DIR:SIMP>
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SIMP)
endif()
set_target_properties(SIMP PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SIMP
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SIMP)
endif()
