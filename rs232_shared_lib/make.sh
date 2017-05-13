#!/bin/sh

clang -Wall -fPIC -g -c main.c
clang -shared -Wl,-soname,librs232.so.1 -o librs232.so.1.0   *.o ../dll_shared_libs/nanovmlib.o ../dll_shared_libs/arr.o ../dll_shared_libs/mutex.o
cp librs232.so.1.0 librs232.so
