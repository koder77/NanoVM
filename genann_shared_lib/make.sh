#!/bin/sh

clang-3.9 -Wall -fPIC -c -g genann.c -O3
clang-3.9 -Wall -fPIC -c -g genann-lib.c -O3 
clang-3.9 -Xlinker -L/usr/local/lib/ -v -shared -Wl,-soname,libgenann.so.1 -o libgenann.so.1.0   *.o ../dll_shared_libs/nanovmlib.o ../dll_shared_libs/arr.o ../dll_shared_libs/mutex.o
cp libgenann.so.1.0 libgenann.so
