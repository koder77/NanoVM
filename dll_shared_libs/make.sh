#!/bin/sh

gcc -Wall -fPIC -c *.c
gcc -shared -Wl,-soname,libtest.so.1 -o libtest.so.1.0   *.o
cp libtest.so.1.0 libtest.so
 