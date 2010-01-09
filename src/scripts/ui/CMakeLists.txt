file(GLOB scriptFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")
install(FILES ${scriptFiles} DESTINATION share/qterm/scripts/ui)
