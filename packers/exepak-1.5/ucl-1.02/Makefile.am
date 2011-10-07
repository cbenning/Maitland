## Process this file with automake to create Makefile.in
#
# Copyright (C) 1996-2003 Markus F.X.J. Oberhumer
#

AUTOMAKE_OPTIONS = 1.7.9 foreign

SUBDIRS = include src examples

dist-hook:
	-rm -rf $(distdir)/acconfig/
	cp -pR $(srcdir)/B/ $(srcdir)/acconfig/ $(srcdir)/asm/ $(srcdir)/upx/ $(distdir)/
	$(mkinstalldirs) $(distdir)/acc
	cp -pR $(srcdir)/acc/ACC* $(srcdir)/acc/acc* $(distdir)/acc/
	find $(distdir)/ -type l -print0 | xargs -0r rm -v -f

@MAINT@-include $(top_srcdir)/Makefile.maint
