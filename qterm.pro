TARGET	= frame

CONFIG	= qt uic resources

MOC_DIR	= .moc

OBJECTS_DIR = .obj

RESOURCES = frame.qrc

FORMS	= ui/frame.ui ui/toolbardialog.ui

SOURCES	= main.cpp	\
	qterm.cpp	\
	qtermframe.cpp	\
	qtermwindowbase.cpp\
	qtermeditor.cpp    \
    toolbardialog.cpp

HEADERS	= qterm.h   \
	qtermframe.h	\
	qtermwindowbase.h\
	qtermeditor.h  \
	toolbardialog.h

