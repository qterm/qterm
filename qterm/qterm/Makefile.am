####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = qterm

## INCLUDES were found outside kdevelop specific part

qterm_SOURCES = soundconf.cpp soundconfui.cpp qtermsound.cpp trayicon.cpp trayicon_win.cpp trayicon_x11.cpp trayicon_mac.cpp schemadialogui.cpp schemadialog.cpp quickdialogui.cpp quickdialog.cpp qtermwndmgr.cpp qtermwindow.cpp qtermtoolbutton.cpp qtermtimelabel.cpp qtermtextline.cpp qtermtelnet.cpp qtermscreen.cpp qtermparam.cpp qtermframe.cpp qtermdecode.cpp qtermconvert.cpp qtermconfig.cpp qtermbuffer.cpp qtermbbs.cpp prefdialogui.cpp prefdialog.cpp popwidget.cpp msgdialogui.cpp msgdialog.cpp main.cpp keydialogui.cpp keydialog.cpp articledialogui.cpp articledialog.cpp addrdialogui.cpp addrdialog.cpp aboutdialogui.cpp aboutdialog.cpp qtermsocket.cpp
SOURCES = soundconf.cpp soundconfui.cpp qtermsound.cpp trayicon.cpp trayicon_win.cpp trayicon_x11.cpp trayicon_mac.cpp schemadialogui.cpp schemadialog.cpp quickdialogui.cpp quickdialog.cpp qtermwndmgr.cpp qtermwindow.cpp qtermtoolbutton.cpp qtermtimelabel.cpp qtermtextline.cpp qtermtelnet.cpp qtermscreen.cpp qtermparam.cpp qtermframe.cpp qtermdecode.cpp qtermconvert.cpp qtermconfig.cpp qtermbuffer.cpp qtermbbs.cpp prefdialogui.cpp prefdialog.cpp popwidget.cpp msgdialogui.cpp msgdialog.cpp main.cpp keydialogui.cpp keydialog.cpp articledialogui.cpp articledialog.cpp addrdialogui.cpp addrdialog.cpp aboutdialogui.cpp aboutdialog.cpp qtermsocket.cpp 

INTERFACES = 
TRANSLATIONS = 
KDE_OPTIONS = qtonly
if WANT_SSL
qterm_LDADD = $(top_builddir)/qterm/ssh/libssh.a $(LIB_QPE) $(LIBPYTHON) $(ESD_LIB) $(LIB_SOUNDSERVER) $(LIB_QT) $(LIBSOCKET) $(LIB_OPENSSL)

SUBDIRS = cursor pic script docs schema ui po ssh
else
qterm_LDADD = $(LIB_QPE) $(LIBPYTHON) $(ESD_LIB) $(LIB_SOUNDSERVER) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = cursor pic script docs schema ui po
endif
EXTRA_DIST = aboutdialog.cpp aboutdialog.h aboutdialogui.cpp aboutdialogui.h addrdialog.cpp addrdialog.h addrdialogui.cpp addrdialogui.h articledialog.cpp articledialog.h articledialogui.cpp articledialogui.h keydialog.cpp keydialog.h keydialogui.cpp keydialogui.h main.cpp msgdialog.cpp msgdialog.h msgdialogui.cpp msgdialogui.h popwidget.cpp popwidget.h prefdialog.cpp prefdialog.h prefdialogui.cpp prefdialogui.h qtermbbs.cpp qtermbbs.h qtermbuffer.cpp qtermbuffer.h qterm.cfg qtermconfig.cpp qtermconfig.h qtermconvert.cpp qtermconvert.h qtermdecode.cpp qtermdecode.h qtermframe.cpp qtermframe.h qterm.h qtermparam.cpp qtermparam.h qtermscreen.cpp qtermscreen.h qtermtelnet.cpp qtermtelnet.h qtermtextline.cpp qtermtextline.h qtermtimelabel.cpp qtermtimelabel.h qtermtoolbutton.cpp qtermtoolbutton.h qtermwindow.cpp qtermwindow.h qtermwndmgr.cpp qtermwndmgr.h quickdialog.cpp quickdialog.h quickdialogui.cpp quickdialogui.h schemadialog.cpp schemadialog.h schemadialogui.cpp schemadialogui.h address.cfg trayicon_mac.cpp trayicon_x11.cpp trayicon_win.cpp trayicon.h trayicon.cpp credits qtermsound.h qtermsound.cpp soundconfui.cpp soundconfui.h soundconf.cpp soundconf.h qterm.prj qterm.rc qterm.ico qtermsocket.h qtermsocket.cpp

install-data-local:
	$(mkinstalldirs) $(datadir)/qterm/
	$(INSTALL_DATA) $(srcdir)/qterm.cfg $(datadir)/qterm/qterm.cfg
	$(mkinstalldirs) $(datadir)/qterm/
	$(INSTALL_DATA) $(srcdir)/address.cfg $(datadir)/qterm/address.cfg
	$(mkinstalldirs) $(datadir)/qterm/
	$(INSTALL_DATA) $(srcdir)/credits $(datadir)/qterm/credits

uninstall-local:
	-rm -f $(datadir)/qterm/qterm.cfg
	-rm -f $(datadir)/qterm/address.cfg
	-rm -f $(datadir)/qterm/credits

####### kdevelop will overwrite this part!!! (end)############


# set the include path for X, qt and KDE
INCLUDES = $(all_includes) $(PYTHONINC) $(SOUNDSERVER_INC) $(ESD_INC) $(OPENSSL_INC)

qterm_METASOURCES = AUTO

# the library search path. 
qterm_LDFLAGS = $(all_libraries) -Wl -export-dynamic $(PYTHONLIB) $(SOUNDSERVER_LIB) $(ESD_LIB) $(OPENSSL_LIB)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(qterm_METASOURCES)

applnkInternet_DATA =  qterm.desktop
applnkInternetdir = $(kde_appdir)

icon_DATA = qterm.png
icondir = $(prefix)/share/icons
