set(optionalLibs)
set(optionalSources)
option(QTERM_ENABLE_SSH "Build SSH Support" ON)
option(QTERM_ENABLE_DBUS "Build DBus Support" ON)
option(QTERM_ENABLE_SCRIPT "Build Script Support" ON)
option(QTERM_ENABLE_PHONON "Build Phonon Support" ON)
option(QTERM_OLD_PHONON "Hack for Outdated Phonon Library" OFF)
option(QTERM_ENABLE_TEST "Build the tests")
option(QTERM_ENABLE_SCRIPT_DEBUGGER "Build ScriptDebugger Support")

include(CheckFunctionExists)
include(CheckLibraryExists)

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
   if(OPENSSL_CRYPTO_FOUND)
      message(STATUS "OpenSSL found, enable SSH support")
      add_definitions(-DSSH_ENABLED)
      include_directories(${OPENSSL_CRYPTO_INCLUDE_DIR})
      set(optionalLibs ${optionalLibs} ${OPENSSL_CRYPTO_LIBRARIES})
      set(optionalSources ${optionalSources}
      ssh/auth.cpp
      ssh/channel.cpp
      ssh/crc32.cpp
      ssh/kex.cpp
      ssh/packet.cpp
      ssh/socket.cpp
      ssh/transport.cpp)
   endif(OPENSSL_CRYPTO_FOUND)
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
   if(QT_QTSCRIPT_FOUND)
      message(STATUS "QtScript module found, enable Script support")
      find_package(QtScriptQtBindings)
      add_definitions(-DSCRIPT_ENABLED)
      include_directories(${QT_QTSCRIPT_INCLUDE_DIR})
      set(optionalLibs ${optionalLibs} ${QT_QTSCRIPT_LIBRARY})
      set(optionalSources ${optionalSources} scripthelper.cpp)
      if(QTERM_ENABLE_SCRIPT_DEBUGGER)
         if(QT_QTSCRIPTTOOLS_FOUND)
            message(STATUS "QtScriptTools module found, enable Debugger support")
            add_definitions(-DSCRIPTTOOLS_ENABLED)
            include_directories(${QT_QTSCRIPTTOOLS_INCLUDE_DIR})
            set(optionalLibs ${optionalLibs} ${QT_QTSCRIPTTOOLS_LIBRARY})
         else(QT_QTSCRIPTTOOLS_FOUND)
            message(STATUS "QtScriptTools module not found, at least CMake 2.8 is needed to support it")
         endif(QT_QTSCRIPTTOOLS_FOUND)
      endif(QTERM_ENABLE_SCRIPT_DEBUGGER)
   endif(QT_QTSCRIPT_FOUND)
endif(QTERM_ENABLE_SCRIPT)
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
   ui_aboutdialog.h
   ui_addrdialog.h
   ui_articledialog.h
   ui_imageviewer.h
   ui_keydialog.h
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
   aboutdialog.cpp
   addrdialog.cpp
   articledialog.cpp
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
   qtermwndmgr.cpp
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
   ${optionalSources})
set(qterm_UIS
   ui/aboutdialog.ui
   ui/addrdialog.ui
   ui/articledialog.ui
   ui/imageviewer.ui
   ui/keydialog.ui
   ui/msgdialog.ui
   ui/prefdialog.ui
   ui/quickdialog.ui
   ui/schemedialog.ui
   ui/shortcutsdialog.ui
   ui/soundconf.ui
   ui/sshlogin.ui
   ui/toolbardialog.ui
   ui/closedialog.ui
   ui/zmodemdialog.ui)
set(qterm_MISC
   address.cfg
   credits
   qterm.cfg)
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
  find_library(WIN32_WS2_LIBRARY NAMES ws2_32)
  if(WIN32_WS2_LIBRARY)
    set(optionalLibs ${optionalLibs} ${WIN32_WS2_LIBRARY})
  endif(WIN32_WS2_LIBRARY)
endif(WIN32)
qt4_wrap_ui(qterm_UIS_H ${qterm_UIS})
qt4_automoc(${qterm_SRCS})
add_definitions( -DHAVE_CONFIG_H )
include_directories(
   ${QT_INCLUDE_DIR}
   ${QT_QTCORE_INCLUDE_DIR}
   ${QT_QTGUI_INCLUDE_DIR}
   ${QT_QTNETWORK_INCLUDE_DIR}
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR})
if(WIN32)
   add_executable(qterm WIN32 ${qterm_SRCS})
else(WIN32)
   add_executable(qterm ${qterm_SRCS})
endif(WIN32)
target_link_libraries(qterm
   ${QT_LIBRARIES}
   ${QT_QTCORE_LIBRARY}
   ${QT_QTGUI_LIBRARY}
   ${QT_QTNETWORK_LIBRARY}
   ${optionalLibs})
if(WIN32 AND MINGW)
   target_link_libraries(qterm ${win32Libs})
endif(WIN32 AND MINGW)
install(TARGETS qterm DESTINATION bin)
install(FILES ${qterm_MISC} DESTINATION share/qterm)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qterm.desktop DESTINATION share/applications)
add_subdirectory(pic)
add_subdirectory(cursor)
add_subdirectory(scheme)
add_subdirectory(scripts)
add_subdirectory(po)
add_subdirectory(keyboard_profiles)
add_dependencies(qterm translations_target)
if(QTERM_ENABLE_TEST)
   add_subdirectory(test)
endif(QTERM_ENABLE_TEST)
