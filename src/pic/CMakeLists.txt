file(GLOB picFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
install(FILES ${picFiles} DESTINATION share/qterm/pic)
