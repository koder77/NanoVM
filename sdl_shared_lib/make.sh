#!/bin/sh

gcc -Wall -fPIC -g -c sdl-main.c
gcc -shared -Wl,-soname,libsdldraw.so.1 -o libsdldraw.so.1.0   *.o ../dll_shared_libs/nanovmlib.o ../dll_shared_libs/arr.o ../dll_shared_libs/mutex.o -lSDL -lSDL_gfx -lSDL_image -lSDL_ttf
cp libsdldraw.so.1.0 libsdldraw.so
