# nano assembler makefile
#
# by Stefan Pietzonke  jay-t@gmx.net
#
#

bin_PROGRAMS = nanoa

# Linker:
nanoa_SOURCES = main.c \
		loadp.c \
		saveo.c \
		saveend.c \
		opcode.c \
		stringf.c \
		var.c \
		jumpl.c \
		ccl.c \
		regname.c

noinst_HEADERS = main_d.h

LDADD = ../lib/libshared.a

AM_CFLAGS = -O0 -g
AM_CPPFLAGS = -I../include
