#!/bin/sh

#rm -rf build/${CROSS_TRIPLE}

${CROSS_TRIPLE}-cmake \
-B"build/${CROSS_TRIPLE}" \
-H. \
-G"Ninja" \
-DBoost_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DOGG_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DVORBIS_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DOGG_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libogg.a" \
-DVORBIS_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libvorbis.a" \
-DVORBISFILE_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libvorbisfile.a" \
-DOPTION_DISABLE_STACKTRACE=On \
$@

cd build/${CROSS_TRIPLE}

ninja -j$(nproc --all)
