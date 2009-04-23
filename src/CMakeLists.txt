set(optionalLibs)
set(optionalSources)
set(win32Libs libwsock32.a)
option(QTERM_ENABLE_SSH "Build SSH Support" ON)
option(QTERM_ENABLE_DBUS "Build DBus Support" ON)
option(QTERM_ENABLE_PHONON "Build Phonon Support" ON)
option(QTERM_ENABLE_TEST "Build the tests")
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
      set(optionalSources ${optionalSources} dbus.cpp)
      find_file(KWALLET_XML org.kde.KWallet.xml ${KDE4_DBUS_INTERFACES_DIR} ${DBUS_INTERFACES_INSTALL_DIR})
      if(KWALLET_XML)
         message(STATUS "KWallet found, enable KWallet support")
         add_definitions(-DKWALLET_ENABLED)
         qt4_add_dbus_interface(optionalSources ${KWALLET_XML} kwallet_interface )
         set(optionalSources ${optionalSources} wallet.cpp)
      endif(KWALLET_XML)
   endif(QT_QTDBUS_FOUND)
endif(QTERM_ENABLE_DBUS)
if(QTERM_ENABLE_PHONON)
   if(QT_PHONON_FOUND)
      message(STATUS "Qt Phonon module found, enable Phonon support")
      add_definitions(-DPHONON_ENABLED)
      include_directories(${QT_PHONON_INCLUDE_DIR})
      set(optionalLibs ${optionalLibs} ${QT_PHONON_LIBRARY})
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
   ui_schemadialog.h
   ui_soundconf.h
   ui_sshlogin.h
   ui_shortcutsdialog.h
   ui_toolbardialog.h
   ui_zmodemdialog.h
   aboutdialog.cpp
   addrdialog.cpp
   articledialog.cpp
   imageviewer.cpp
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
   schemadialog.cpp
   shortcutsdialog.cpp
   statusBar.cpp
   toolbardialog.cpp
   zmodemdialog.cpp
   hostinfo.cpp
   termstring.cpp
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
   ui/schemadialog.ui
   ui/shortcutsdialog.ui
   ui/soundconf.ui
   ui/sshlogin.ui
   ui/toolbardialog.ui
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
endif(WIN32)
qt4_wrap_ui(qterm_UIS_H ${qterm_UIS})
qt4_automoc(${qterm_SRCS})
add_definitions( -Wall -DHAVE_CONFIG_H )
if(WIN32 AND MINGW)
   set(CMAKE_CXX_FLAGS -mwindows)
endif(WIN32 AND MINGW)
include_directories(
   ${QT_INCLUDE_DIR}
   ${QT_QTCORE_INCLUDE_DIR}
   ${QT_QTGUI_INCLUDE_DIR}
   ${QT_QTNETWORK_INCLUDE_DIR}
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(qterm ${qterm_SRCS})
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
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qterm.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/qterm.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qterm.desktop DESTINATION share/applications)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/qterm.png DESTINATION share/icons)
add_subdirectory(pic)
add_subdirectory(cursor)
add_subdirectory(schema)
if(QTERM_ENABLE_TEST)
   add_subdirectory(test)
endif(QTERM_ENABLE_TEST)
