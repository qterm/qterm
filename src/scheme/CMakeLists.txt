file(GLOB schemeFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.scheme")
file(COPY ${schemeFiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(FILES ${schemeFiles} DESTINATION share/qterm/scheme)
