set(optionalLibs)
set(optionalSources)
option(QTERM_ENABLE_SSH "Build SSH Support" ON)
option(QTERM_ENABLE_DBUS "Build DBus Support" ON)
option(QTERM_ENABLE_SCRIPT "Build Script Support" ON)
option(QTERM_ENABLE_QMEDIAPLAYER "Build QMediaPlayer Support" ON)
option(QTERM_ENABLE_PHONON "Build Phonon Support" ON)
option(QTERM_OLD_PHONON "Hack for Outdated Phonon Library" OFF)
option(QTERM_ENABLE_TEST "Build the tests")
option(QTERM_ENABLE_SCRIPT_DEBUGGER "Build ScriptDebugger Support")

include(CheckFunctionExists)
include(CheckLibraryExists)

macro (_install_plugins)
   set(options "")
   set(oneValueArgs "DIRECTORY")
   set(multiValueArgs "MODULES")
   cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

   set(plugins)
   foreach(MODULE ${ARG_MODULES})
      if (QT_VERSION_MAJOR VERSION_LESS 6)
         list(APPEND plugins ${Qt${QT_VERSION_MAJOR}${MODULE}_PLUGINS})
      else ()
         get_target_property(_plugins Qt${QT_VERSION_MAJOR}::${MODULE} QT_PLUGINS)
         if (_plugins)
            foreach (_plugin ${_plugins})
               list(APPEND plugins Qt${QT_VERSION_MAJOR}::${_plugin})
            endforeach ()
         endif()
      endif()
   endforeach()

   foreach(plugin ${plugins})
      get_target_property(_loc ${plugin} LOCATION)
      get_filename_component(_plugindir ${_loc} PATH)
      get_filename_component(_pluginname ${_plugindir} NAME)
      install(CODE "
         file(COPY ${_loc} DESTINATION \${CMAKE_INSTALL_PREFIX}/${ARG_DIRECTORY}/${_pluginname})
      " COMPONENT Runtime)
   endforeach()
endmacro()

check_function_exists("gethostbyname" CMAKE_HAVE_GETHOSTBYNAME)
if(NOT CMAKE_HAVE_GETHOSTBYNAME)
   check_library_exists("nsl" "gethostbyname" "" CMAKE_LIB_NSL_HAS_GETHOSTBYNAME)
   if(CMAKE_LIB_NSL_HAS_GETHOSTBYNAME)
      set(optionalLibs ${optionalLibs} -lnsl)
   else(CMAKE_LIB_NSL_HAS_GETHOSTBYNAME)
      check_library_exists("bsd" "gethostbyname" "" CMAKE_LIB_BSD_HAS_GETHOSTBYNAME)
      if(CMAKE_LIB_BSD_HAS_GETHOSTBYNAME)
         set(optionalLibs ${optionalLibs} -lbsd)
      endif(CMAKE_LIB_BSD_HAS_GETHOSTBYNAME)
   endif(CMAKE_LIB_NSL_HAS_GETHOSTBYNAME)
endif(NOT CMAKE_HAVE_GETHOSTBYNAME)

if(QTERM_ENABLE_SSH)
   find_package(OpenSSLCrypto)
   if(OPENSSLCRYPTO_FOUND)
      message(STATUS "OpenSSL found, enable SSH support")
      add_definitions(-DSSH_ENABLED)
      add_definitions(-DOPENSSL_API_COMPAT=10100)
      include_directories(${OPENSSLCRYPTO_INCLUDE_DIR})
      get_filename_component(OPENSSLCRYPTO_ROOT_DIR ${OPENSSLCRYPTO_INCLUDE_DIR} PATH)
      set(optionalLibs ${optionalLibs} ${OPENSSLCRYPTO_LIBRARIES})
      set(optionalSources ${optionalSources}
      ssh/libcrypto-compat.c
      ssh/key.cpp
      ssh/auth.cpp
      ssh/channel.cpp
      ssh/crc32.cpp
      ssh/kex.cpp
      ssh/packet.cpp
      ssh/socket.cpp
      ssh/transport.cpp)
   endif(OPENSSLCRYPTO_FOUND)
endif(QTERM_ENABLE_SSH)

if(QTERM_ENABLE_DBUS)
   if(QT_QTDBUS_FOUND)
      message(STATUS "QtDBus module found, enable DBus support")
      add_definitions(-DDBUS_ENABLED)
      include_directories(${QT_QTDBUS_INCLUDE_DIR})
      set(optionalLibs ${optionalLibs} ${QT_QTDBUS_LIBRARY})
      set(optionalSources ${optionalSources}
      imageconverter.cpp
      dbus.cpp)
      find_file(KWALLET_XML org.kde.KWallet.xml ${KDE4_DBUS_INTERFACES_DIR} ${DBUS_INTERFACES_INSTALL_DIR})
      if(KWALLET_XML)
         message(STATUS "KWallet found, enable KWallet support")
         add_definitions(-DKWALLET_ENABLED)
         qt4_add_dbus_interface(optionalSources ${KWALLET_XML} kwallet_interface )
         set(optionalSources ${optionalSources} wallet.cpp)
      endif(KWALLET_XML)
   endif(QT_QTDBUS_FOUND)
endif(QTERM_ENABLE_DBUS)

if(QTERM_ENABLE_SCRIPT)
   if(Qt5Script_FOUND)
      message(STATUS "QtScript module found, enable Script support")
      add_definitions(-DSCRIPT_ENABLED)
      include_directories(${Qt5Script_INCLUDE_DIRS})
      set(optionalLibs ${optionalLibs} ${Qt5Script_LIBRARIES})
      set(optionalSources ${optionalSources} scripthelper.cpp)
      if(QTERM_ENABLE_SCRIPT_DEBUGGER)
            if(Qt5ScriptTools_FOUND)
               message(STATUS "QtScriptTools module found, enable Debugger support")
               add_definitions(-DSCRIPTTOOLS_ENABLED)
               include_directories(${Qt5ScriptTools_INCLUDE_DIRS})
               set(optionalLibs ${optionalLibs} ${Qt5ScriptTools_LIBRARIES})
            else(Qt5ScriptTools_FOUND)
               message(STATUS "QtScriptTools module not found")
            endif(Qt5ScriptTools_FOUND)
      endif(QTERM_ENABLE_SCRIPT_DEBUGGER)
   endif(Qt5Script_FOUND)
endif(QTERM_ENABLE_SCRIPT)

if(QTERM_ENABLE_QMEDIAPLAYER)
   if(Qt5Multimedia_FOUND)
      message(STATUS "Qt Multimedia module found, enable QMediaPlayer support")
      add_definitions(-DQMEDIAPLAYER_ENABLED)
      include_directories(${Qt5Multimedia_INCLUDE_DIRS})
      set(optionalLibs ${optionalLibs} ${Qt5Multimedia_LIBRARIES})
   endif(Qt5Multimedia_FOUND)
endif(QTERM_ENABLE_QMEDIAPLAYER)

if(QTERM_ENABLE_PHONON)
   if(QT_PHONON_FOUND)
      message(STATUS "Qt Phonon module found, enable Phonon support")
      add_definitions(-DPHONON_ENABLED)
      include_directories(${QT_PHONON_INCLUDE_DIR})
      set(optionalLibs ${optionalLibs} ${QT_PHONON_LIBRARY})
      if(QTERM_OLD_PHONON OR QT_VERSION_MINOR LESS 5)
         add_definitions(-DOLD_PHONON)
      endif(QTERM_OLD_PHONON OR QT_VERSION_MINOR LESS 5)
   else(QT_PHONON_FOUND)
      # Looking for KDE4, it is quite impossible that some system can have
      # Phonon without KDE4
      if(PHONON_FOUND)
         message(STATUS "KDE Phonon module found, enable Phonon support")
         add_definitions(-DPHONON_ENABLED)
         include_directories(${KDE4_INCLUDES})
         set(optionalLibs ${optionalLibs} ${KDE4_PHONON_LIBRARY})
      endif(PHONON_FOUND)
   endif(QT_PHONON_FOUND)
endif(QTERM_ENABLE_PHONON)

set(qterm_SRCS
   aboutdialog.cpp
   addrdialog.cpp
   articledialog.cpp
   assistantclient.cpp
   imageviewer.cpp
   imagelistviewer.cpp
   keydialog.cpp
   main.cpp
   msgdialog.cpp
   osdmessage.cpp
   overlayWidget.cpp
   popupMessage.cpp
   popwidget.cpp
   prefdialog.cpp
   progressBar.cpp
   qtermbbs.cpp
   qtermbuffer.cpp
   qtermcanvas.cpp
   qtermconfig.cpp
   qtermconvert.cpp
   qtermdecode.cpp
   qtermframe.cpp
   qtermhttp.cpp
   qtermiplocation.cpp
   qtermparam.cpp
   qtermscreen.cpp
   qtermsocket.cpp
   qtermsound.cpp
   qtermtelnet.cpp
   qtermtextline.cpp
   qtermtoolbutton.cpp
   qtermwindow.cpp
   qtermwindowbase.cpp
   qtermzmodem.cpp
   qtermglobal.cpp
   quickdialog.cpp
   schemedialog.cpp
   shortcutsdialog.cpp
   statusBar.cpp
   toolbardialog.cpp
   zmodemdialog.cpp
   hostinfo.cpp
   termstring.cpp
   keyboardtranslator.cpp
   closedialog.cpp
   uaocodec.cpp
   dommodel.cpp
   pallete.cpp
   chartable.cpp
   charpane.cpp
   blur.cpp
   ${optionalSources})
set(qterm_UIS
   ui/aboutdialog.ui
   ui/addrdialog.ui
   ui/articledialog.ui
   ui/closedialog.ui
   ui/imageviewer.ui
   ui/keydialog.ui
   ui/mainframe.ui
   ui/msgdialog.ui
   ui/prefdialog.ui
   ui/quickdialog.ui
   ui/schemedialog.ui
   ui/shortcutsdialog.ui
   ui/soundconf.ui
   ui/sshlogin.ui
   ui/toolbardialog.ui
   ui/zmodemdialog.ui
   ui/pallete.ui)
set(qterm_UIS_H
   ui_aboutdialog.h
   ui_addrdialog.h
   ui_articledialog.h
   ui_imageviewer.h
   ui_keydialog.h
   ui_mainframe.h
   ui_msgdialog.h
   ui_prefdialog.h
   ui_quickdialog.h
   ui_schemedialog.h
   ui_soundconf.h
   ui_sshlogin.h
   ui_shortcutsdialog.h
   ui_toolbardialog.h
   ui_zmodemdialog.h
   ui_closedialog.h
   ui_pallete.h)
set(qterm_MISC
   address.xml
   credits
   qterm.cfg)
set(qterm_RCCS
    qterm.qrc)

file(COPY ${qterm_MISC} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

find_program(XDGOPEN NAME xdg-open DOC "Looking for xdg-open")
if (XDGOPEN_FOUND)
  set (HTTP xdg-open)
else (XDGOPEN_FOUND)
  set (HTTP)
endif (XDGOPEN_FOUND)

configure_file(${CMAKE_CURRENT_BINARY_DIR}/qterm.cfg ${CMAKE_CURRENT_BINARY_DIR}/qterm.cfg)

if(WIN32)
  if(MINGW)
    # resource compilation for mingw
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/qterm_rc.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR}
                                           -i${CMAKE_CURRENT_SOURCE_DIR}/qterm.rc
                                           -o ${CMAKE_CURRENT_BINARY_DIR}/qterm_rc.o)
    SET(qterm_SRCS ${qterm_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/qterm_rc.o)
  else(MINGW)
    SET(qterm_SRCS ${qterm_SRCS} qterm.rc)
  endif(MINGW)
  set(optionalLibs ${optionalLibs} ws2_32)
  cmake_policy(SET CMP0020 NEW) # automatically link to qtmain.lib
endif(WIN32)

if (QT_VERSION_MAJOR EQUAL 5)
  qt5_add_resources(qterm_RCCS_SRCS ${qterm_RCCS})
  qt5_wrap_ui(qterm_UIS_H ${qterm_UIS})
else ()
  qt_add_resources(qterm_RCCS_SRCS ${qterm_RCCS})
  # https://bugreports.qt.io/browse/QTBUG-93413
  if (QT_VERSION VERSION_LESS 6.7.1)
    qt_wrap_ui(qterm_UIS_H ${qterm_UIS} OPTIONS -c string)
  else ()
    qt_wrap_ui(qterm_UIS_H ${qterm_UIS})
  endif ()
endif ()

set(qterm_SRCS ${qterm_SRCS} ${qterm_RCCS} ${qterm_UIS})
add_compile_definitions( -DHAVE_CONFIG_H )
include_directories(
    ${Qt${QT_VERSION_MAJOR}Qt_INCLUDE_DIRS}
    ${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS}
    ${Qt${QT_VERSION_MAJOR}Core_INCLUDE_DIRS}
    ${Qt${QT_VERSION_MAJOR}Network_INCLUDE_DIRS}
    )
    if(${QT_VERSION_MAJOR} VERSION_EQUAL 6)
        include_directories(${Qt${QT_VERSION_MAJOR}Core5Compat_INCLUDE_DIRS})
    endif()
if(UNIX AND NOT APPLE)
   find_package(X11)
   include_directories(${X11_INCLUDE_DIR})
   set(optionalLibs ${optionalLibs} ${X11_X11_LIB})
endif(UNIX AND NOT APPLE)
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR})
if(WIN32)
   add_executable(${PROGRAME} WIN32 ${qterm_SRCS} ${qterm_RCCS_SRCS})
elseif(APPLE)
   set(qterm_MISC  ${qterm_MISC} ${CMAKE_CURRENT_SOURCE_DIR}/../icons/QTerm.icns)
   set_source_files_properties(${qterm_MISC} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
   add_executable(${PROGRAME} MACOSX_BUNDLE ${qterm_SRCS} ${qterm_RCCS_SRCS} ${qterm_MISC} )
   add_custom_command( TARGET ${PROGRAME} PRE_BUILD
     COMMAND mkdir ARGS -p ${PROGRAME}.app/Contents/MacOS
     COMMAND mkdir ARGS -p ${PROGRAME}.app/Contents/Resources
     COMMAND mkdir ARGS -p ${PROGRAME}.app/Contents/Resources/po
     COMMAND mkdir ARGS -p ${PROGRAME}.app/Contents/Resources/doc
     COMMAND mkdir ARGS -p ${PROGRAME}.app/Contents/Resources/keyboard_profiles
   )
   add_custom_command( TARGET ${PROGRAME} POST_BUILD
     COMMAND cp ARGS ${CMAKE_CURRENT_BINARY_DIR}/po/*.qm ${PROGRAME}.app/Contents/Resources/po
   #  COMMAND cp ARGS ${CMAKE_CURRENT_BINARY_DIR}/doc/*.{qhc,qch} ${PROGRAME}.app/Contents/Resources/doc
     COMMAND cp ARGS ${CMAKE_CURRENT_BINARY_DIR}/keyboard_profiles/*.keytab ${PROGRAME}.app/Contents/Resources/keyboard_profiles
   )
else()
   add_executable(${PROGRAME} ${qterm_SRCS} ${qterm_RCCS_SRCS})
endif()
target_link_libraries(${PROGRAME}
    ${Qt${QT_VERSION_MAJOR}Core_LIBRARIES}
    ${Qt${QT_VERSION_MAJOR}Gui_LIBRARIES}
    ${Qt${QT_VERSION_MAJOR}Widgets_LIBRARIES}
    ${Qt${QT_VERSION_MAJOR}Xml_LIBRARIES}
    ${Qt${QT_VERSION_MAJOR}PrintSupport_LIBRARIES}
    ${Qt${QT_VERSION_MAJOR}Network_LIBRARIES}
   ${optionalLibs})
if (${QT_VERSION_MAJOR} VERSION_EQUAL 6)
target_link_libraries(${PROGRAME}
     ${Qt6Core5Compat_LIBRARIES})
ENDIF()

if(WIN32 AND MINGW)
   target_link_libraries(${PROGRAME} ${win32Libs})
endif(WIN32 AND MINGW)
if (APPLE)
   install(TARGETS ${PROGRAME} DESTINATION .)
   _install_plugins(
      MODULES Gui Widgets
      DIRECTORY ${PROGRAME}.app/Contents/PlugIns
   )
   install(CODE "
      file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${PROGRAME}.app/Contents/Resources/qt.conf\" \"[Paths]\nPlugIns=PlugIns\")
   " COMPONENT Runtime)
   install(CODE "
      file(GLOB_RECURSE PLUGINS  \${CMAKE_INSTALL_PREFIX}/${PROGRAME}.app/Contents/PlugIns/*${CMAKE_SHARED_LIBRARY_SUFFIX})
      set(BU_CHMOD_BUNDLE_ITEMS ON)
      include(BundleUtilities)
      fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${PROGRAME}.app/Contents/MacOS/QTerm\"
       \"\${PLUGINS}\"
       \"${QT_LIBRARY_DIR}\")
   " COMPONENT Runtime)
else (APPLE)
  install(TARGETS ${PROGRAME} DESTINATION bin)
  install(FILES ${qterm_MISC} DESTINATION share/qterm)
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qterm.desktop DESTINATION share/applications)
  if(WIN32)
      _install_plugins (
         MODULES Gui Widgets
         DIRECTORY plugins
      )
      install(CODE "
         file(WRITE \"\${CMAKE_INSTALL_PREFIX}/bin/qt.conf\" \"[Paths]\nPlugIns=../plugins\")
      " COMPONENT Runtime)
      install(CODE "
         file(GLOB_RECURSE PLUGINS  \${CMAKE_INSTALL_PREFIX}/bin/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX})
         include(BundleUtilities)
         set(DIRS \"${QT_BINARY_DIR}\" \"${QT_LIBRARY_DIR}\" \"${OPENSSLCRYPTO_ROOT_DIR}\")
         fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/bin/${PROGRAME}.exe\"
         \"\${PLUGINS}\"
         \"\${DIRS}\")
      ")
  endif(WIN32)
endif(APPLE)
add_subdirectory(scheme)
add_subdirectory(scripts)
add_subdirectory(po)
add_subdirectory(keyboard_profiles)
add_subdirectory(doc)
add_dependencies(${PROGRAME} translations_target help_target)
if(QTERM_ENABLE_TEST)
   add_subdirectory(test)
endif(QTERM_ENABLE_TEST)
