#!/bin/sh

echo "running configure..."
export CC=clang CXX=clang++ && ./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6 -march=native -s" LDFLAGS="-lrt"
#! ./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6 -march=native" LDFLAGS="-lrt"
echo "running make..."
make
