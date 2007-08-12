file(GLOB schemaFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.schema")
install(FILES ${schemaFiles} DESTINATION share/qterm/schema)
