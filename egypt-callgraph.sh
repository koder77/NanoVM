#!/bin/sh

make clean
./configure CFLAGS="-fdump-rtl-expand"
make

egypt vm/*.expand lib/*.expand | dot -Grankdir=LR -Tps -o callgraph.ps

make clean
