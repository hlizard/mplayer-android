export NDKHOME=/android/android-ndk-r5b
export NDKHOME=/opt/android-ndk
export CFLAGS="-O9 -DANDROID -march=armv6 -ffast-math -fomit-frame-pointer -nostdlib -lc -lm  -L $NDKHOME/platforms/android-9/arch-arm/usr/lib/   -I $NDKHOME/platforms/android-9/arch-arm/usr/include/ -mandroid  -march=armv6 -mtune=xscale -funroll-loops -Isdl -ldl"
export CFLAGS="-O3 -DANDROID -ffast-math -fomit-frame-pointer -nostdlib -lc -lm  -L $NDKHOME/platforms/android-9/arch-arm/usr/lib/   -I $NDKHOME/platforms/android-9/arch-arm/usr/include/ -mandroid  -march=armv7-a -mfpu=neon -mtune=xscale -funroll-loops -Isdl -ldl"
