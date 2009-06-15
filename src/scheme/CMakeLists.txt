file(GLOB schemeFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.scheme")
install(FILES ${schemeFiles} DESTINATION share/qterm/scheme)
