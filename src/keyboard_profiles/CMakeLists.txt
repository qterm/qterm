file(GLOB cursorFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.keytab")
install(FILES ${cursorFiles} DESTINATION share/qterm/keyboard_profiles)
