#!/bin/sh

echo "running configure..."
./configure CFLAGS="-s -march=armv6 -mtune=arm1176jzf-s -mfloat-abi=hard -mfpu=vfp -ffast-math -Wcast-align -fno-strict-aliasing -fomit-frame-pointer -O3" LDFLAGS="-lrt"
echo "running make..."
make
