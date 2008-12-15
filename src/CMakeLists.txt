set(optionalLibs)
set(optionalSources)
set(win32Libs libwsock32.a)
option(QTERM_ENABLE_SSH "Build SSH Support" ON)
option(QTERM_ENABLE_DBUS "Build DBus Support" ON)
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
   endif(QT_QTDBUS_FOUND)
endif(QTERM_ENABLE_DBUS)
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
qt4_wrap_ui(qterm_UIS_H ${qterm_UIS})
qt4_automoc(${qterm_SRCS})
add_definitions( -Wall -DHAVE_CONFIG_H )
if(WIN32)
   set(CMAKE_CXX_FLAGS -mwindows)
endif(WIN32)
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
if(WIN32)
   target_link_libraries(qterm ${win32Libs})
endif(WIN32)
install(TARGETS qterm DESTINATION bin)
install(FILES ${qterm_MISC} DESTINATION share/qterm)
add_subdirectory(pic)
add_subdirectory(cursor)
add_subdirectory(schema)
if(QTERM_ENABLE_TEST)
   add_subdirectory(test)
endif(QTERM_ENABLE_TEST)
