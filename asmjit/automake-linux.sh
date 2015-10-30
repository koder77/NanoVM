#!/bin/sh

cd build

echo "running cmake..."
export CC=clang-3.7 CXX=clang++-3.7 ASMJIT_EMBED=1 && cmake ../
make
