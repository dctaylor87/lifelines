# This makefile creates the B-Tree database subsystem

noinst_LIBRARIES = libbtree.a

libbtree_a_SOURCES = \
	addkey.c \
	block.c \
	btrec.c \
	file.c \
	index.c \
	opnbtree.c \
	traverse.c \
	utils.c \
	btreei.h

# $(top_builddir)        for config.h
# $(top_srcdir)/src/hdrs for headers

AM_CPPFLAGS=-I$(top_builddir) -I$(top_srcdir)/src/hdrs

EXTRA_DIST = btreei.h

