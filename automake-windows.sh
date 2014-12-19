#!/bin/sh

echo "running configure..."
./configure CFLAGS="-ffast-math -fomit-frame-pointer -O3 -s" LIBS="$LIBS -L../lib -lwsock32 -lws2_32 -lpthread"
echo "running make..."
make
