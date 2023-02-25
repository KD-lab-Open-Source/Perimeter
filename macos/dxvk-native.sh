#!/bin/sh

echo $(pwd)

ARCH=$1
shift
CROSS_ROOT=$1
shift

echo "Cross compiling DXVK-Native for $ARCH at $CROSS_ROOT with $@"

export CFLAGS="-arch $ARCH"
export CXXFLAGS="-arch $ARCH"
export LDFLAGS="-arch $ARCH"

exec meson setup \
--cross-file "$CROSS_ROOT/meson-$ARCH.txt" \
--cross-file $CROSS_ROOT/meson-config.txt \
-Ddxvk_native_force=true \
$@