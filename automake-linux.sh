#!/bin/sh

echo "running configure..."
./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6 -march=core2" LDFLAGS="-lrt"
#!./configure CFLAGS="-ffast-math -fomit-frame-pointer -O3" LDFLAGS="-lrt"
echo "running make..."
make
