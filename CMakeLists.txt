project(qterm)
include(CTest)
cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
set(QTERM_VERSION_MAJOR 0)
set(QTERM_VERSION_MINOR 6)
set(QTERM_VERSION_RELEASE 0)
set(QTERM_VERSION "${QTERM_VERSION_MAJOR}.${QTERM_VERSION_MINOR}.${QTERM_VERSION_RELEASE}" )
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
IF(QT5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Xml REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5PrintSupport REQUIRED)
    find_package(Qt5LinguistTools REQUIRED)
    find_package(Qt5Script)
    find_package(Qt5ScriptTools)
    find_package(Qt5Multimedia)
    IF(NOT QT_BINARY_DIR)
        exec_program(${_qt5Core_install_prefix}/bin/qmake ARGS "-query QT_INSTALL_BINS"
            OUTPUT_VARIABLE QT_BINARY_DIR)
    ENDIF()
ELSE(QT5)
    find_package(Qt4 4.5.0 REQUIRED)
    find_package(KDE4)
ENDIF(QT5)

IF(WIN32 AND MSVC)
    ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32 AND MSVC)

IF(APPLE)
   SET (PROGRAME QTerm)
   SET (MACOSX_BUNDLE_VERSION $QTERM_VERSION)
   SET (MACOSX_BUNDLE_ICON_FILE QTerm.icns)
ELSE(APPLE)
   SET (PROGRAME qterm)
ENDIF(APPLE)

include("package.cmake")
add_subdirectory(src)
add_subdirectory(icons)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
if (NOT TARGET uninstall)
    add_custom_target(uninstall
      "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif(NOT TARGET uninstall)
