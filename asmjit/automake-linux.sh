#!/bin/sh

cd build

echo "running cmake..."
export CC=clang-3.6 CXX=clang++-3.6 ASMJIT_STATIC=1 && cmake ../
make
