#!/bin/sh

clang-3.9 -Wall -fPIC -c *.c
clang-3.9 -shared -Wl,-soname,libtest.so.1 -o libtest.so.1.0   *.o
cp libtest.so.1.0 libtest.so
 
