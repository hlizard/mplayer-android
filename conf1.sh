set -x
. ./confn.sh
# export CFLAGS="-g"
export CC="$NDKHOME/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gcc --sysroot=/opt/android-ndk/platforms/android-9/arch-arm/"
echo $CC
./configure --ar=$NDKHOME/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-ar --as=$NDKHOME/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-as --ranlib=$NDK_HOME/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-ranlib --host-cc=gcc  --target=arm-linux-eabi  --disable-dvdnav --disable-dvdread --disable-dvdread-internal --disable-vcd --disable-real --disable-xanim --disable-mencoder  --disable-mp3lib  --disable-tremor-internal  --disable-fbdev --disable-tga --disable-md5sum --disable-yuv4mpeg --disable-pnm --disable-dvb --disable-tv --disable-ftp --disable-select --disable-armv6t2  --enable-mpg123 --disable-gif --disable-jpeg --enable-faad --disable-libmpeg2-internal --disable-mencoder --datadir="/sdcard/mplayer" --confdir="/sdcard/mplayer" --libdir="/sdcard/mplayer" --disable-armvfp --disable-inet6
