file(GLOB keyboardFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.keytab")
file(COPY ${keyboardFiles} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(FILES ${keyboardFiles} DESTINATION share/qterm/keyboard_profiles)
