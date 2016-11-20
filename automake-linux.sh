#!/bin/sh

echo "running configure..."
export CC=clang-3.6 CXX=clang++-3.6 && ./configure CFLAGS="-ffast-math -fomit-frame-pointer -O3 -march=native -s" LDFLAGS="-lrt"
#! ./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6 -march=native" LDFLAGS="-lrt"
echo "running make..."
make
