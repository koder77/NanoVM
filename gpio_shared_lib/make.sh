#!/bin/sh

clang -Wall -fPIC -g -c gpio-main.c
clang -shared -Wl,-soname,libgpio.so.1 -o libgpio.so.1.0   *.o ../dll_shared_libs/nanovmlib.o ../dll_shared_libs/arr.o ../dll_shared_libs/mutex.o -lwiringPi
cp libgpio.so.1.0 libgpio.so
