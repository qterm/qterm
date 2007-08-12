file(GLOB cursorFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.xpm")
install(FILES ${cursorFiles} DESTINATION share/qterm/cursor)
