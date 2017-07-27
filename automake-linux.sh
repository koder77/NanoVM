#!/bin/sh

echo "running configure..."
export CC=clang-3.9 CXX=clang++-3.9 && ./configure CFLAGS="-ffast-math -fomit-frame-pointer -g0 -O3 -march=native -s" LDFLAGS="-lrt"
#! ./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6 -march=native" LDFLAGS="-lrt"
echo "running make..."
make
