project(qterm)
include(CTest)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)
set(QT_MIN_VERSION "4.5.0")
set(QTERM_VERSION_MAJOR 0)
set(QTERM_VERSION_MINOR 5)
set(QTERM_VERSION_RELEASE 12)
set(QTERM_VERSION "${QTERM_VERSION_MAJOR}.${QTERM_VERSION_MINOR}.${QTERM_VERSION_RELEASE}" )
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Qt4 REQUIRED)
find_package(KDE4)

IF(WIN32 AND MSVC)
    ADD_DEFINITIONS (/D _CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32 AND MSVC)

add_subdirectory(src)
add_subdirectory(icons)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include("package.cmake")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
