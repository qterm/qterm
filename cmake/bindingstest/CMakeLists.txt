cmake_minimum_required (VERSION 2.6)

find_package( Qt4 REQUIRED )

add_definitions(${QT_DEFINITIONS})
include_directories (${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTSCRIPT_INCLUDE_DIR})

add_executable (bindingstest QtScriptBindingsTest.cpp)
target_link_libraries(bindingstest ${QT_LIBRARY} ${QT_QTSCRIPT_LIBRARY})

