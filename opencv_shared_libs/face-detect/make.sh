#!/bin/sh

g++ -Wall -fPIC -c facedet_video.cpp
gcc -shared -Wl,-soname,libopencvfacedet.so.1 -o libopencvfacedet.so.1.0   *.o ../../dll_shared_libs/nanovmlib.o ../../dll_shared_libs/arr.o ../../dll_shared_libs/mutex.o -lopencv_video -lopencv_core -lopencv_videostab -lopencv_contrib -lopencv_highgui
cp libopencvfacedet.so.1.0 libopencvfacedet.so
 