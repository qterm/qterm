INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_NAME "QTerm")
SET(CPACK_PACKAGE_VENDOR "The QTerm Project")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QTerm - The BBS Client")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.rst")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT")
SET(CPACK_PACKAGE_VERSION_MAJOR ${QTERM_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${QTERM_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${QTERM_VERSION_RELEASE})
IF(WIN32)
  SET(CPACK_PACKAGE_EXECUTABLES "qterm;QTerm")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\qterm.exe")
  SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/src/qterm.ico")
  SET(CPACK_NSIS_HELP_LINK "http://www.qterm.org")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.qterm.org")
  SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
  SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    CreateShortCut \\\"$DESKTOP\\\\QTerm.lnk\\\" \\\"$INSTDIR\\\\bin\\\\qterm.exe\\\"
  ")

  SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    Delete \\\"$DESKTOP\\\\QTerm.lnk\\\"
  ")
ELSEIF(APPLE)
  SET(CPACK_GENERATOR "DragNDrop")
ELSE()
  SET(CPACK_STRIP_FILES "bin/qterm")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF()

# Functions to install qt, ssl libraries
#FUNCTION(install_libs, lib
INCLUDE(CPack)

