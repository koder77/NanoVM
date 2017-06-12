#!/bin/sh

clang -Wall -fPIC -g -c i2c-main.c
clang -shared -Wl,-soname,libi2c.so.1 -o libi2c.so.1.0   *.o ../dll_shared_libs/nanovmlib.o ../dll_shared_libs/arr.o ../dll_shared_libs/mutex.o -lwiringPi
cp libi2c.so.1.0 libi2c.so
