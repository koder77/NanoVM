# nano vm makefile
#
# by Stefan Pietzonke  jay-t@gmx.net
#
#

bin_PROGRAMS = nanovm

nanovm_SOURCES = main.c \
	       openo.c \
	       openend.c \
	       exe_arr.c \
	       exe_file.c \
	       exe_main.c \
	       exe_socket.c \
	       stringf.c \
	       mt19937ar.c \
	       exe_process.c \
	       verifyo.c \
	       exe_stack.c \
	       rinzler.c \
	       hash32.c \
	       hash_64a.c \
	       pthread.c \
	       snprintf.c \
	       dynamic_array.c \
	       strtoll.c \
	       exe_vector_arr.c \
	       exe_passw_input.c \
	       hyperspace_server.c \
	       hyperspace_client.c \
	       jit.cpp \
	       rights.c \
	       dlload.c \
	       rs232.c

noinst_HEADERS = main_d.h \
		 arch.h \
		 exe_socket_ipv4_amiga.h \
		 exe_socket_ipv6.h \
		 fnv.h \
		 longlong.h \
		 snprintf.h \
		 jit_x86.h

#LDADD = ../lib/libshared.a -ldl ../asmjit/build/libasmjit.a
LDADD = ../lib/libshared.a -ldl
#LDADD = ../lib/libshared.a

AM_CFLAGS = -O3 -g0 -lm

AM_CPPFLAGS = -I../include

AM_LDFLAGS =-Wl,--export-dynamic
