#!/bin/sh

echo "running configure..."
./configure CFLAGS="-fomit-frame-pointer -O3 -s -I/usr/local/include" LIBS="$LIBS -L../lib -L/usr/local/lib -lmingw32 -lSDLmain -lSDL -lSDL_mixer -lSDL_ttf -lSDL_gfx -lSDL_net -lSDL_image -lwsock32 -lws2_32"
echo "running make..."
make
