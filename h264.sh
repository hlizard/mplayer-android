#!/bin/sh

set -x 

/opt/android-ndk/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc --sysroot=/opt/android-ndk/platforms/android-9/arch-arm/  -o ./libh264.so --shared libmpcodecs/vd_h264.o -nostdlib -ldl -lgcc -lm -lc -llog -lz -L/opt/android-ndk/platforms/android-9/arch-arm/usr/lib/ -rdynamic -Wl,-z,noexecstack
