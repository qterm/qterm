set(hostinfo_SRCS
   testhostinfo.cpp
   ../../hostinfo.cpp)
qt4_automoc(${hostinfo_SRCS})

include_directories(
${QT_INCLUDE_DIR}
${QT_QTCORE_INCLUDE_DIR}
${QT_QTGUI_INCLUDE_DIR}
${QT_QTNETWORK_INCLUDE_DIR}
${QT_QTTEST_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR})

add_executable(testhostinfo ${hostinfo_SRCS})

target_link_libraries(testhostinfo
${QT_LIBRARIES}
${QT_QTCORE_LIBRARY}
${QT_QTGUI_LIBRARY}
${QT_QTTEST_LIBRARY}
)

add_test(hostinfo ${EXEC_DIR}/testhostinfo)
