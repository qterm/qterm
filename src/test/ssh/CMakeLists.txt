set(ssh_SRCS
   ../../ssh/auth.cpp
   ../../ssh/channel.cpp
   ../../ssh/crc32.cpp
   ../../ssh/kex.cpp
   ../../ssh/packet.cpp
   ../../ssh/socket.cpp
   ../../ssh/transport.cpp
   ../../qtermsocket.cpp
   ../../hostinfo.cpp
   testssh.cpp)
qt4_automoc(${ssh_SRCS})

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

add_executable(testssh ${ssh_SRCS})

target_link_libraries(testssh
${QT_LIBRARIES}
${QT_QTCORE_LIBRARY}
${QT_QTGUI_LIBRARY}
${QT_QTNETWORK_LIBRARY}
${QT_QTTEST_LIBRARY}
${OPENSSL_LIBRARIES}
)

add_test(ssh ${EXEC_DIR}/testssh)
