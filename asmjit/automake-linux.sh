#!/bin/sh

echo "running configure..."
./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6"
#!./configure CFLAGS="-ffast-math -fomit-frame-pointer -O3"
echo "running make..."
make
