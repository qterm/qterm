TARGET	= frame

CONFIG	= qt uic resources

MOC_DIR	= .moc

OBJECTS_DIR = .obj

RESOURCES = frame.qrc

FORMS	= ui/frame.ui ui/toolbardialog.ui ui/shortcutsdialog.ui

SOURCES	= main.cpp	\
	qterm.cpp	\
	qtermframe.cpp	\
	qtermwindowbase.cpp\
	qtermeditor.cpp    \
    	toolbardialog.cpp \
    	shortcutsdialog.cpp

HEADERS	= qterm.h   \
	qtermframe.h	\
	qtermwindowbase.h\
	qtermeditor.h  \
	toolbardialog.h \
	shortcutsdialog.h

