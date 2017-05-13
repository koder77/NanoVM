#!/bin/sh

clang -Wall -fPIC -c *.c
clang -shared -Wl,-soname,libtest.so.1 -o libtest.so.1.0   *.o
cp libtest.so.1.0 libtest.so
 