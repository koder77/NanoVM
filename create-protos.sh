#!/bin/bash

rm include/protos.h

cproto assemb/*.c -I include >>include/protos.h
cproto lib/*.c -I include >>include/protos.h
cproto comp/*.c -I include >>include/protos.h
cproto	vm/main.c \
	       vm/openo.c \
	       vm/openend.c \
	       vm/exe_arr.c \
	       vm/exe_file.c \
	       vm/exe_main.c \
	       vm/exe_socket.c \
	       vm/stringf.c \
	       vm/mt19937ar.c \
	       vm/exe_process.c \
	       vm/verifyo.c \
	       vm/exe_stack.c \
	       vm/rinzler.c \
	       vm/hash32.c \
	       vm/pthread.c \
	       vm/snprintf.c \
	       vm/dynamic_array.c \
	       vm/strtoll.c \
	       vm/exe_vector_arr.c \
	       vm/exe_passw_input.c \
	       vm/hyperspace_server.c \
	       vm/hyperspace_client.c \
	       vm/jit.cpp \
	       vm/rights.c \
	       vm/dlload.c -I include >>include/protos.h
	       