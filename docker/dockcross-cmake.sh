#!/bin/bash

set -e

echo "Triplet: ${CROSS_TRIPLE}"
if [[ $CROSS_TRIPLE == *shared ]]; then
  LIB_SUFFIX=.dll.a
else
  LIB_SUFFIX=.a
fi
if [[ $CROSS_TRIPLE == x86_64* ]]; then
  IS_64=1
fi

#rm -rf build/${CROSS_TRIPLE}

${CROSS_TRIPLE}-cmake \
-B"build/${CROSS_TRIPLE}" \
-H. \
-G"Ninja" \
-DBoost_INCLUDE_DIR="/usr/local/include" \
-DOGG_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DVORBIS_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DSDL2_INCLUDE_DIR="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include/SDL2" \
-DLIBRARY_SEARCH_PATHS="/usr/lib/mxe/usr/${CROSS_TRIPLE}/include" \
-DOGG_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libogg${LIB_SUFFIX}" \
-DVORBIS_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libvorbis${LIB_SUFFIX}" \
-DVORBISFILE_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libvorbisfile${LIB_SUFFIX}" \
-DSDL2_LIBRARY="/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libSDL2main.a;/usr/lib/mxe/usr/${CROSS_TRIPLE}/lib/libSDL2${LIB_SUFFIX}" \
-DOPTION_LINKER_LLD=OFF \
$@

cd build/${CROSS_TRIPLE}

ninja -j$(nproc --all)

mkdir -p output

cp Source/perimeter.exe output
#cp /usr/lib/mxe/usr/${CROSS_TRIPLE}/bin/SDL2.dll output
cp -fv /usr/lib/mxe/usr/${CROSS_TRIPLE}/bin/libogg*.dll output
cp -fv /usr/lib/mxe/usr/${CROSS_TRIPLE}/bin/libvorbis*.dll output
cp -fv /usr/lib/mxe/usr/${CROSS_TRIPLE}/bin/libvorbisfile*.dll output
cp -fv /usr/lib/mxe/usr/${CROSS_TRIPLE}/bin/libstdc*.dll output
cp -fv /usr/lib/mxe/usr/${CROSS_TRIPLE}/bin/libgcc*.dll output
cp -fv /usr/lib/mxe/usr/${CROSS_TRIPLE}/bin/libwinpthread-*.dll output

exit 0
