#!/bin/bash

echo "Triplet: ${CROSS_TRIPLE}"
if [[ $CROSS_TRIPLE == *shared ]]; then
  LIB_SUFFIX=.dll.a
else
  LIB_SUFFIX=.a
fi

#rm -rf build/${CROSS_TRIPLE}

${CROSS_TRIPLE}-cmake \
-B"build/${CROSS_TRIPLE}" \
-H. \
-G"Ninja" \
-DBoost_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DOGG_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DVORBIS_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DSDL2_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include/SDL2" \
-DLIBRARY_SEARCH_PATHS="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DOGG_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libogg${LIB_SUFFIX}" \
-DVORBIS_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libvorbis${LIB_SUFFIX}" \
-DVORBISFILE_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libvorbisfile${LIB_SUFFIX}" \
-DSDL2_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libSDL2main.a;/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libSDL2${LIB_SUFFIX}" \
-DOPTION_DISABLE_STACKTRACE=On \
$@

cd build/${CROSS_TRIPLE}

ninja -j$(nproc --all)

exit 0
