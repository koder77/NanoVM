#!/bin/sh

echo "running configure..."
export CC=clang-3.7 CXX=clang++-3.7 && ./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6"
#! ./configure CFLAGS="-ffast-math -fomit-frame-pointer -O6"
echo "running make..."
make
