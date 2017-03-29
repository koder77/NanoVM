#!/bin/sh

cd build

echo "running cmake..."
export CC=clang CXX=clang++ ASMJIT_STATIC=TRUE ASMJIT_EMBED=TRUE ASMJIT_RELEASE=TRUE && cmake ../
make
