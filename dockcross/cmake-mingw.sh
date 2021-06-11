#!/bin/sh

#rm -rf build/${CROSS_TRIPLE}

${CROSS_TRIPLE}-cmake \
-B"build/${CROSS_TRIPLE}" \
-H. \
-G"Ninja" \
-DBoost_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
$@

cd build/${CROSS_TRIPLE}

ninja -j$(nproc --all)
