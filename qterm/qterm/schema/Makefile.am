####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = Softness.schema Linux.schema VIM.schema XTerm.schema 

install-data-local:
	$(mkinstalldirs) $(datadir)/qterm/schema/
	$(INSTALL_DATA) $(srcdir)/Softness.schema $(datadir)/qterm/schema/Softness.schema
	$(mkinstalldirs) $(datadir)/qterm/schema/
	$(INSTALL_DATA) $(srcdir)/Linux.schema $(datadir)/qterm/schema/Linux.schema
	$(mkinstalldirs) $(datadir)/qterm/schema/
	$(INSTALL_DATA) $(srcdir)/VIM.schema $(datadir)/qterm/schema/VIM.schema
	$(mkinstalldirs) $(datadir)/qterm/schema/
	$(INSTALL_DATA) $(srcdir)/XTerm.schema $(datadir)/qterm/schema/XTerm.schema

uninstall-local:
	-rm -f $(datadir)/qterm/schema/Softness.schema
	-rm -f $(datadir)/qterm/schema/Linux.schema
	-rm -f $(datadir)/qterm/schema/VIM.schema
	-rm -f $(datadir)/qterm/schema/XTerm.schema

####### kdevelop will overwrite this part!!! (end)############
