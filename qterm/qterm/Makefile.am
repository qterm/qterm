####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qterm

## INCLUDES were found outside kdevelop specific part

qterm_SOURCES = qtermpythonembed.cpp schemadialogui.cpp schemadialog.cpp quickdialogui.cpp quickdialog.cpp qtermwndmgr.cpp qtermwindow.cpp qtermtoolbutton.cpp qtermtimelabel.cpp qtermtextline.cpp qtermtelnet.cpp qtermscreen.cpp qtermparam.cpp qtermframe.cpp qtermdecode.cpp qtermconvert.cpp qtermconfig.cpp qtermbuffer.cpp qtermbbs.cpp prefdialogui.cpp prefdialog.cpp popwidget.cpp msgdialogui.cpp msgdialog.cpp main.cpp keydialogui.cpp keydialog.cpp articledialogui.cpp articledialog.cpp addrdialogui.cpp addrdialog.cpp aboutdialogui.cpp aboutdialog.cpp 
SOURCES = qtermpythonembed.cpp schemadialogui.cpp schemadialog.cpp quickdialogui.cpp quickdialog.cpp qtermwndmgr.cpp qtermwindow.cpp qtermtoolbutton.cpp qtermtimelabel.cpp qtermtextline.cpp qtermtelnet.cpp qtermscreen.cpp qtermparam.cpp qtermframe.cpp qtermdecode.cpp qtermconvert.cpp qtermconfig.cpp qtermbuffer.cpp qtermbbs.cpp prefdialogui.cpp prefdialog.cpp popwidget.cpp msgdialogui.cpp msgdialog.cpp main.cpp keydialogui.cpp keydialog.cpp articledialogui.cpp articledialog.cpp addrdialogui.cpp addrdialog.cpp aboutdialogui.cpp aboutdialog.cpp 
INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
qterm_LDADD   =  $(LIB_QPE) $(LIB_QT) $(LIBSOCKET) $(LIBPYTHON)

SUBDIRS = cursor pic 

EXTRA_DIST = aboutdialog.cpp aboutdialog.h aboutdialogui.cpp aboutdialogui.h addrdialog.cpp addrdialog.h addrdialogui.cpp addrdialogui.h articledialog.cpp articledialog.h articledialogui.cpp articledialogui.h keydialog.cpp keydialog.h keydialogui.cpp keydialogui.h main.cpp msgdialog.cpp msgdialog.h msgdialogui.cpp msgdialogui.h popwidget.cpp popwidget.h prefdialog.cpp prefdialog.h prefdialogui.cpp prefdialogui.h qtermbbs.cpp qtermbbs.h qtermbuffer.cpp qtermbuffer.h qterm.cfg qtermconfig.cpp qtermconfig.h qtermconvert.cpp qtermconvert.h qtermdecode.cpp qtermdecode.h qtermframe.cpp qtermframe.h qterm.h qtermparam.cpp qtermparam.h qtermscreen.cpp qtermscreen.h qtermtelnet.cpp qtermtelnet.h qtermtextline.cpp qtermtextline.h qtermtimelabel.cpp qtermtimelabel.h qtermtoolbutton.cpp qtermtoolbutton.h qtermwindow.cpp qtermwindow.h qtermwndmgr.cpp qtermwndmgr.h quickdialog.cpp quickdialog.h quickdialogui.cpp quickdialogui.h schemadialog.cpp schemadialog.h schemadialogui.cpp schemadialogui.h address.cfg qtermpythonembed.cpp 

install-data-local:
	$(mkinstalldirs) $(datadir)/
	$(INSTALL_DATA) $(srcdir)/qterm.cfg $(datadir)/qterm.cfg
	$(mkinstalldirs) $(datadir)/
	$(INSTALL_DATA) $(srcdir)/address.cfg $(datadir)/address.cfg

uninstall-local:
	-rm -f $(datadir)/qterm.cfg
	-rm -f $(datadir)/address.cfg

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes) $(PYTHONINC)

qterm_METASOURCES = AUTO

# the library search path. 
qterm_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qterm_METASOURCES)

