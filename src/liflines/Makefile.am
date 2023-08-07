# Process this file with automake to produce Makefile.in
# (configure will process the resulting Makefile.in to produce Makefile)

# what are we going to build?
bin_PROGRAMS = llines llexec

# what goes into lines?
llines_SOURCES = add.c advedit.c ask.c askgedc.c askprogram.c \
	browse.c brwsmenu.c \
	choose.c cscurses.c delete.c dynmenu.c \
	edit.c export.c import.c interact.c \
	lbrowse.c lines_usage.c listui.c loadsave.c \
	main.c menuset.c merge.c miscutls.c \
	newrecs.c pedigree.c \
	scan.c screen.c searchui.c selectdb.c show.c swap.c \
	tandem.c valgdcom.c \
	listui.h llinesi.h 

# what goes into llexec ?
llexec_SOURCES = ask.c choose.c lines_usage.c llexec.c \
	selectdb.c miscutls.c llexec.h

PYTHON_LIBS = @PYTHON_LIBS@

# extra stuff for lines
llines_LDADD = ../interp/libinterp.a @LIBPYLLINES@ ../gedlib/libgedcom.a \
	../btree/libbtree.a ../stdlib/libstdlib.a ../arch/libarch.a \
	../ui/libui.a \
	$(PYTHON_LIBS) @LIBINTL@ @LIBICONV@

# extra stuff for llexec
llexec_LDADD = ../interp/libinterp.a @LIBPYLLINES@ ../gedlib/libgedcom.a \
	../btree/libbtree.a ../stdlib/libstdlib.a ../arch/libarch.a \
	../ui/libuicli.a \
	$(PYTHON_LIBS) @LIBINTL@ @LIBICONV@

# $(top_builddir)        for config.h
# $(top_srcdir)/src/hdrs for headers

# Flags needed to compile Python code
PYTHON_CPPFLAGS = @PYTHON_CPPFLAGS@

AM_CPPFLAGS=-I$(top_builddir) -I$(top_srcdir)/src/hdrs $(PYTHON_CPPFLAGS)

# Locale information for gettext
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

EXTRA_DIST = llinesi.h screeni.h

# these dependencies are so that building parts ('make llines' or 'make
# llexec') will work

../btree/libbtree.a :
	$(MAKE) -C ../btree libbtree.a

../interp/libinterp.a :
	$(MAKE) -C ../interp libinterp.a

../gedlib/libgedcom.a :
	$(MAKE) -C ../gedlib libgedcom.a

../python/libpyllines.a :
	$(MAKE) -C ../python libpyllines.a

../stdlib/libstdlib.a :
	$(MAKE) -C ../stdlib libstdlib.a

../ui/libuicli.a :
	$(MAKE) -C ../ui libuicli.a

../ui/libui.a :
	$(MAKE) -C ../ui libui.a
