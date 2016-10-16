#!/bin/sh

g++ -Wall -fPIC -c robotvision.cpp
gcc -shared -Wl,-soname,librobotvision.so.1 -o librobotvision.so.1.0   *.o ../../dll_shared_libs/nanovmlib.o ../../dll_shared_libs/arr.o ../../dll_shared_libs/mutex.o -L/usr/lib/x86_64-linux-gnu -lopencv_video -lopencv_core -lopencv_videostab -lopencv_contrib -lopencv_highgui
cp librobotvision.so.1.0 librobotvision.so
 