####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = continuous.py download.py system.py tools.py sendmsg.py

install-data-local:
	$(mkinstalldirs) $(datadir)/qterm/script/
	$(INSTALL_DATA) $(srcdir)/continuous.py $(datadir)/qterm/script/continuous.py
	$(mkinstalldirs) $(datadir)/qterm/script/
	$(INSTALL_DATA) $(srcdir)/download.py $(datadir)/qterm/script/download.py
	$(mkinstalldirs) $(datadir)/qterm/script/
	$(INSTALL_DATA) $(srcdir)/system.py $(datadir)/qterm/script/system.py
	$(mkinstalldirs) $(datadir)/qterm/script/
	$(INSTALL_DATA) $(srcdir)/system.py $(datadir)/qterm/script/tools.py
	$(mkinstalldirs) $(datadir)/qterm/script/
	$(INSTALL_DATA) $(srcdir)/system.py $(datadir)/qterm/script/sendmsg.py

uninstall-local:
	-rm -f $(datadir)/qterm/script/continuous.py
	-rm -f $(datadir)/qterm/script/download.py
	-rm -f $(datadir)/qterm/script/system.py
	-rm -f $(datadir)/qterm/script/tools.py
	-rm -f $(datadir)/qterm/script/sendmsg.py


####### kdevelop will overwrite this part!!! (end)############
