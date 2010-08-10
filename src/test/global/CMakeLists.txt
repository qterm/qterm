set(global_SRCS
   testglobal.cpp
   ../../qtermconfig.cpp
   ../../qtermglobal.cpp
   ../../qtermparam.cpp
   ../../qtermconvert.cpp)
qt4_automoc(${global_SRCS})
remove_definitions(-DKWALLET_ENABLED)

include_directories(
${QT_INCLUDE_DIR}
${QT_QTCORE_INCLUDE_DIR}
${QT_QTTEST_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}
${CMAKE_BINARY_DIR}
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_CURRENT_SOURCE_DIR})

add_executable(testglobal ${global_SRCS})

target_link_libraries(testglobal
${QT_LIBRARIES}
${QT_QTCORE_LIBRARY}
${QT_QTGUI_LIBRARY}
${QT_QTNETWORK_LIBRARY}
${QT_QTTEST_LIBRARY}
${QT_QTXML_LIBRARY}
)

add_test(global ${EXEC_DIR}/testglobal)
