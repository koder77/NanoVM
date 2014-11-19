# nano flow makefile
#
# by Stefan Pietzonke  jay-t@gmx.net
#
#

bin_PROGRAMS = flow2

# Linker:
flow2_SOURCES = main.c \
		socket.c \
		gadgets.c \
		draw.c \
		string.c \
		mem.c \
		file.c \
		loadscan.c \
		apps.c

noinst_HEADERS = global_d.h \
		socket.h

AM_CFLAGS = -O0 -g
# AM_CPPFLAGS = -I../include

SUBDIRS =

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath
