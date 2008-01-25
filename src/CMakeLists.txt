set( qterm_SRCS
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
qtermpython.cpp
qtermscreen.cpp
qtermsocket.cpp
qtermsound.cpp
qtermtelnet.cpp
qtermtextline.cpp
#qtermtimelabel.cpp
qtermtoolbutton.cpp
qtermwindow.cpp
qtermwndmgr.cpp
qtermzmodem.cpp
quickdialog.cpp
schemadialog.cpp
soundconf.cpp
statusBar.cpp
#trayicon.cpp
#trayicon_mac.cpp
#trayicon_win.cpp
#trayicon_x11.cpp
zmodemdialog.cpp
ssh/crc32.cpp
ssh/qtermsshauth.cpp
ssh/qtermsshbuffer.cpp
ssh/qtermsshdes.cpp
ssh/qtermsshkex.cpp
ssh/qtermsshlogin.cpp
ssh/qtermsshmd5.cpp
ssh/qtermsshpacket.cpp
ssh/qtermsshrsa.cpp
ssh/qtermsshsession.cpp
ssh/qtermsshsocket.cpp )
set( qterm_UIS
ui/aboutdialog.ui
ui/addrdialog.ui
ui/articledialog.ui
ui/imageviewer.ui
ui/keydialog.ui
ui/msgdialog.ui
ui/prefdialog.ui
ui/quickdialog.ui
ui/schemadialog.ui
ui/soundconf.ui
ui/sshlogin.ui
ui/zmodemdialog.ui )
set( qterm_MISC
address.cfg
credits
qterm.cfg )
qt4_wrap_ui( qterm_UIS_H ${qterm_UIS} )
qt4_automoc(${qterm_SRCS})
ADD_DEFINITIONS( -Wall -DHAVE_CONFIG_H )
include_directories(
${QT_INCLUDE_DIR}
${QT_QTCORE_INCLUDE_DIR}
${QT_QTGUI_INCLUDE_DIR}
${QT_QTNETWORK_INCLUDE_DIR}
${OPENSSL_INCLUDE_DIR}
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
${OPENSSL_LIBRARIES}
)
install(TARGETS qterm DESTINATION bin)
install(FILES ${qterm_MISC} DESTINATION share/qterm)
add_subdirectory(pic)
add_subdirectory(cursor)
add_subdirectory(schema)
