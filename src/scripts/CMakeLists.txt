file(GLOB scriptFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.js")
install(FILES ${scriptFiles} DESTINATION share/qterm/scripts)
