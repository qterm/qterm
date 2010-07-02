CONFIGURE_FILE(../pic/qterm.png qterm.png COPYONLY)
CONFIGURE_FILE(qterm.qhp.in  qterm.qhp)
CONFIGURE_FILE(index.html.in index.html)

FIND_PROGRAM(QT_HELP_COLLECTION_GENERATOR
  qcollectiongenerator
  PATHS "${QT_BINARY_DIR}"
  DOC "qcollectiongenerator used to compile Qt help collection project files")


ADD_CUSTOM_COMMAND(
  OUTPUT qterm.qhc
  DEPENDS qterm.qhcp
  DEPENDS qterm.qhp
  COMMAND ${QT_HELP_COLLECTION_GENERATOR}
  ARGS qterm.qhcp
      -o qterm.qhc
  COMMENT "Compiling Qt help collection project qterm.qhcp"
)

ADD_CUSTOM_TARGET(help_target
  DEPENDS qterm.qhc)

INSTALL(
    FILES qterm.qhc
    DESTINATION share/qterm/doc
)

