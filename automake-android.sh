#!/bin/sh
export TOOLCHAIN=$HOME/build/arm-linux-androideabi-4.8
export ANDROID_NDK=$TOOLCHAIN
export PATH=$TOOLCHAIN/bin:$PATH
export SYSROOT=$TOOLCHAIN
export CONFIG_SYSROOT=$TOOLCHAIN
export CROSS_COMPILE="arm-linux-androideabi"
export CC=$CROSS_COMPILE-gcc
export CXX=$CROSS_COMPILE-g++
export CPP=$CROSS_COMPILE-cpp

#! -mstructure-size-boundary=8
export CFLAGS="-marm -march=armv7-a -mfloat-abi=softfp -Wcast-align -fno-strict-aliasing -O0 -pie -I$TOOLCHAIN/sysroot/usr/include"
export LDFLAGS="-marm -march=armv7-a -Wl,--fix-cortex-a8 -llog -lc -pie -L$TOOLCHAIN/sysroot/usr/lib"
echo "Compiler set up for ARM ABI 17"
echo "running configure..."
./configure-android --host=arm-linux-androideabi --prefix=$SYSROOT/usr/ 
echo "running make..."
make
