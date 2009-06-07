file(GLOB TRANSLATIONS_FILES *.ts)
qt4_add_translation(QM_FILES ${TRANSLATIONS_FILES})
add_custom_target(translations_target DEPENDS ${QM_FILES})
install(FILES ${QM_FILES} DESTINATION share/qterm/po)
