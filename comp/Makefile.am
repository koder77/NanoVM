# nano compiler makefile
#
# by Stefan Pietzonke  jay-t@gmx.net
#
#

bin_PROGRAMS = nanoc

nanoc_SOURCES = parser.c \
	       stringf.c \
	       compiler.c \
	       opcode.c \
	       emit.c \
	       var_comp.c \
	       if.c \
	       for.c \
	       translate.c \
	       optimizer.c \
	       regspill.c

noinst_HEADERS = parser.h \
	       comp_opcode.h \
	       protos.h


LDADD = ../assemb/jumpl.o \
	../assemb/var.o \
	../assemb/loadp.o \
	../lib/libshared.a

AM_CFLAGS = -O0 -g

AM_CPPFLAGS = -I../include


