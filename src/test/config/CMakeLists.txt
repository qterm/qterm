set(config_SRCS
   testconfig.cpp
   ../../qtermconfig.cpp)
qt4_automoc(${config_SRCS})

include_directories(
${QT_INCLUDE_DIR}
${QT_QTCORE_INCLUDE_DIR}
${QT_QTGUI_INCLUDE_DIR}
${QT_QTTEST_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR})

add_executable(testconfig ${config_SRCS})

target_link_libraries(testconfig
${QT_LIBRARIES}
${QT_QTCORE_LIBRARY}
${QT_QTGUI_LIBRARY}
${QT_QTTEST_LIBRARY}
)

add_test(config ${EXEC_DIR}/testconfig)
