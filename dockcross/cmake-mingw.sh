#!/bin/sh

rm -rf build

${CROSS_TRIPLE}-cmake \
-Bbuild \
-H. \
-G"Ninja" \
-DBoost_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
$@

cd build

ninja -j$(nproc --all)
