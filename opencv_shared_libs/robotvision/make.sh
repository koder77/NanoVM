#!/bin/sh

clang++-3.9 -Wall -fPIC -c robotvision-7.cpp -O3
clang++-3.9 -Wall -fPIC -c robotmemory.cpp -O3
clang++-3.9 -Wall -fPIC -c stereo_match.cpp -O3
clang++-3.9 -Xlinker -L/usr/local/lib/ -v -shared -Wl,-soname,librobotvision.so.1 -o librobotvision.so.1.0   *.o ../../dll_shared_libs/nanovmlib.o ../../dll_shared_libs/arr.o ../../dll_shared_libs/mutex.o -lopencv_video -lopencv_core -lopencv_videostab -lopencv_contrib -lopencv_highgui
cp librobotvision.so.1.0 librobotvision.so
 