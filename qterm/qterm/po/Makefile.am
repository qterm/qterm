####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = qterm_chs.qm qterm_chs.ts qterm_cht.qm qterm.cht.ts

install-data-local:
	$(mkinstalldirs) $(datadir)/qterm/po/
	$(INSTALL_DATA) $(srcdir)/qterm_chs.qm $(datadir)/qterm/po/qterm_chs.qm
	$(mkinstalldirs) $(datadir)/qterm/po/
	$(INSTALL_DATA) $(srcdir)/qterm_cht.qm $(datadir)/qterm/po/qterm_cht.qm

uninstall-local:
	-rm -f $(datadir)/qterm/po/qterm_chs.qm
	-rm -f $(datadir)/qterm/po/qterm_cht.qm

####### kdevelop will overwrite this part!!! (end)############
