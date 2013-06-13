file(GLOB scriptFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")
file(COPY ${scriptFiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(FILES ${scriptFiles} DESTINATION share/qterm/scripts/ui)
