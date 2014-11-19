# nano flow makefile
#
# by Stefan Pietzonke  jay-t@gmx.net
#
#

bin_PROGRAMS = flow

# Linker:
flow_SOURCES = main.c \
		mem.c \
		gadgets.c \
		socket.c \
		string.c \
		sound.c \
		rs232.c \
		file.c \
		loadscan.c \
		raspberry-pi.c


noinst_HEADERS = global_d.h \
		rs232.h \
		socket.h

AM_CFLAGS = -O0 -g
# AM_CPPFLAGS = -I../include
