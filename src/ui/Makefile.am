# Makefile for the LifeLines report interpreter subsystem

noinst_LIBRARIES = libuicli.a libui.a

libui_core_SOURCES = print.c \
	curses-ui.c stdio-ui.c \
	uiioi.h

libui_a_SOURCES = $(libui_core_SOURCES)

libuicli_a_SOURCES = ui_cli.c $(libui_core_SOURCES)

# $(top_srcdir)          for config.h
# $(top_srcdir)/src/hdrs for headers

AM_CPPFLAGS=-I$(top_srcdir) -I$(top_srcdir)/src/hdrs

