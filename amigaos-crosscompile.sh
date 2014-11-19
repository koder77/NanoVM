#!/bin/bash
export CC=m68k-amigaos-gcc
export AS=m68k-amigaos-as
export LD=m68k-amigaos-ld
export AR=m68k-amigaos-ar
export RANLIB=m68k-amigaos-ranlib
export STRIP=m68k-amigaos-strip
./configure CFLAGS="-m68020 -fomit-frame-pointer -O2 -I/usr/local/amiga/m68k-amigaos/include/ -D__amigaos__ -D__mc68000__ \
`pthread-config --cflags` \
-DPTHREAD_EVERYWHERE" \
LDFLAGS="`pthread-config --ldflags`" \
LIBS="`pthread-config --libs`" \
--host=i686-pc-linux-gnu
make
