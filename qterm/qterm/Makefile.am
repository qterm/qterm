####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qterm

## INCLUDES were found outside kdevelop specific part

qterm_SOURCES = msgdialogui.cpp schemadialogui.cpp schemadialog.cpp popwidget.cpp quickdialogui.cpp qtermtoolbutton.cpp qtermparam.cpp qtermtimelabel.cpp qtermframe.cpp prefdialogui.cpp qtermconfig.cpp qtermbuffer.cpp qtermbbs.cpp qtermconvert.cpp quickdialog.cpp prefdialog.cpp keydialogui.cpp keydialog.cpp articledialogui.cpp articledialog.cpp addrdialogui.cpp aboutdialogui.cpp aboutdialog.cpp qtermwndmgr.cpp qtermwindow.cpp qtermtextline.cpp qtermtelnet.cpp qtermscreen.cpp qtermdecode.cpp msgdialog.cpp main.cpp addrdialog.cpp 
SOURCES = msgdialogui.cpp schemadialogui.cpp schemadialog.cpp popwidget.cpp quickdialogui.cpp qtermtoolbutton.cpp qtermparam.cpp qtermtimelabel.cpp qtermframe.cpp prefdialogui.cpp qtermconfig.cpp qtermbuffer.cpp qtermbbs.cpp qtermconvert.cpp quickdialog.cpp prefdialog.cpp keydialogui.cpp keydialog.cpp articledialogui.cpp articledialog.cpp addrdialogui.cpp aboutdialogui.cpp aboutdialog.cpp qtermwndmgr.cpp qtermwindow.cpp qtermtextline.cpp qtermtelnet.cpp qtermscreen.cpp qtermdecode.cpp msgdialog.cpp main.cpp addrdialog.cpp 
INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
qterm_LDADD   =  $(LIB_QT) -lXext -lX11 $(LIBSOCKET)

SUBDIRS = pic cursor po 

EXTRA_DIST = addrdialog.cpp addrdialog.h main.cpp msgdialog.cpp msgdialog.h qterm.cfg qterm.h qtermdecode.cpp qtermdecode.h qtermscreen.cpp qtermscreen.h qtermtelnet.cpp qtermtelnet.h qtermtextline.cpp qtermtextline.h qtermwindow.cpp qtermwindow.h qtermwndmgr.cpp qtermwndmgr.h qterm.pro aboutdialog.cpp aboutdialog.h aboutdialogui.cpp aboutdialogui.h addrdialogui.cpp addrdialogui.h articledialog.cpp articledialog.h articledialogui.cpp articledialogui.h keydialog.cpp keydialog.h keydialogui.cpp keydialogui.h prefdialog.cpp quickdialogui.h quickdialog.cpp qtermconvert.cpp qtermbbs.cpp qtermbbs.h qtermbuffer.cpp qtermbuffer.h qtermconfig.cpp qtermconfig.h qtermconvert.h prefdialog.h prefdialogui.cpp prefdialogui.h qtermframe.cpp qtermframe.h qtermtimelabel.cpp qtermparam.cpp qtermparam.h qtermtimelabel.h qtermtoolbutton.h qtermtoolbutton.cpp quickdialogui.cpp quickdialog.h address.cfg popwidget.cpp popwidget.h schemadialog.cpp schemadialog.h schemadialogui.cpp schemadialogui.h msgdialogui.cpp msgdialogui.h credits.txt 

install-data-local:
	$(mkinstalldirs) $(datadir)/qterm/
	$(INSTALL_DATA) $(srcdir)/qterm.cfg $(datadir)/qterm/qterm.cfg
	$(mkinstalldirs) $(datadir)/qterm/
	$(INSTALL_DATA) $(srcdir)/address.cfg $(datadir)/qterm/address.cfg
	$(mkinstalldirs) $(datadir)/qterm/
	$(INSTALL_DATA) $(srcdir)/credits.txt $(datadir)/qterm/credits.txt

uninstall-local:
	-rm -f $(datadir)/qterm/qterm.cfg
	-rm -f $(datadir)/qterm/address.cfg
	-rm -f $(datadir)/qterm/credits.txt

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

qterm_METASOURCES = AUTO

# the library search path. 
qterm_LDFLAGS = $(all_libraries) 

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qterm_METASOURCES)

