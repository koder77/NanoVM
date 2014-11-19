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
	       register.c \
	       exe_vector_arr.c \
	       exe_passw_input.c \
	       hyperspace_server.c \
	       hyperspace_client.c \
	       jit.cpp \
	       rights.c

noinst_HEADERS = main_d.h \
		 arch.h \
		 exe_socket_ipv4_amiga.h \
		 exe_socket_ipv6.h \
		 fnv.h \
		 longlong.h \
		 snprintf.h \
		 jit_x86.h

LDADD = ../lib/libshared.a ../asmjit/libAsmJit.a
#LDADD = ../lib/libshared.a

AM_CFLAGS = -O0 -g -lm

AM_CPPFLAGS = -I../include
