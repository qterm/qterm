TEMPLATE	= app
CONFIG		+= qt warn_on debug
win32:DEFINES += QT_DLL
OBJECTS_DIR 	= obj
MOC_DIR		= moc
HEADERS		= qtermframe.h \
		qtermwindow.h \
		qtermtimelabel.h \
		qtermtoolbutton.h	\
		qtermwndmgr.h	\
		qtermscreen.h	\
		qtermtelnet.h	\
		qtermconfig.h	\
		qtermtextline.h	\
		qtermconvert.h	\
		qtermbuffer.h	\
		qtermdecode.h	\
		qtermbbs.h	\
		qtermparam.h	\
		qterm.h		\
		aboutdialogui.h	\
		aboutdialog.h	\
		addrdialogui.h	\
		addrdialog.h	\
		quickdialogui.h	\
		quickdialog.h	\
		prefdialogui.h	\
		prefdialog.h	\
		keydialogui.h		\
		keydialog.h		\
		msgdialogui.h	\
		msgdialog.h		\
		articledialogui.h	\
		articledialog.h	\
		schemadialogui.h	\
		schemadialog.h	\
		popwidget.h
		
SOURCES		= qtermframe.cpp \
		qtermwindow.cpp \
		qtermtimelabel.cpp \
		qtermtoolbutton.cpp	\
		qtermwndmgr.cpp \
		qtermscreen.cpp	\
		qtermtelnet.cpp	\
		qtermconfig.cpp	\
		qtermtextline.cpp	\
		qtermconvert.cpp	\
		qtermbuffer.cpp	\
		qtermdecode.cpp	\
		qtermbbs.cpp	\
		qtermparam.cpp	\
		aboutdialogui.cpp	\
		aboutdialog.cpp	\
		addrdialogui.cpp	\
		addrdialog.cpp	\
		quickdialogui.cpp	\
		quickdialog.cpp	\
		prefdialogui.cpp	\
		prefdialog.cpp	\
		keydialogui.cpp	\
		keydialog.cpp	\
		msgdialogui.cpp	\
		msgdialog.cpp	\
		articledialogui.cpp	\
		articledialog.cpp	\
		schemadialogui.cpp	\
		schemadialog.cpp	\
		popwidget.cpp	\
		main.cpp
		
TARGET		= qterm
