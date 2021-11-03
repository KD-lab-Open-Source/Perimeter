#!/bin/bash

set -e

if [[ -z $TEMPLATE_DIR ]]; then
  echo "TEMPLATE_DIR missing"
  exit 1
fi
if [[ -z $BUILD_DIR ]]; then
  echo "BUILD_DIR missing"
  exit 1
fi
if [[ -z $APP_DIR ]]; then
  echo "APP_DIR missing"
  exit 1
fi
if [[ -z $LIB_DIR ]]; then
  echo "LIB_DIR missing"
  exit 1
fi

VULKAN_LIB_DIR="/usr/local/lib"
DXVK_NATIVE_PATH="$BUILD_DIR/Source/dxvk-native-prefix/src/dxvk-native-build/src/d3d9/libdxvk_d3d9.dylib"

cp -r $TEMPLATE_DIR $APP_DIR

mkdir $APP_DIR/Contents/Frameworks
mkdir $APP_DIR/Contents/Libs
mkdir $APP_DIR/Contents/MacOS

cp -a $LIB_DIR/libSDL2-2.0.0.dylib $APP_DIR/Contents/Libs/
cp -a $LIB_DIR/libSDL2_image-2.0.0.dylib $APP_DIR/Contents/Libs/
cp -a $LIB_DIR/libSDL2_mixer-2.0.0.dylib $APP_DIR/Contents/Libs/
cp -a $LIB_DIR/libSDL2_net-2.0.0.dylib $APP_DIR/Contents/Libs/

cp -a $LIB_DIR/libavcodec.58*.dylib $APP_DIR/Contents/Libs/
cp -a $LIB_DIR/libavformat.58*.dylib $APP_DIR/Contents/Libs/
cp -a $LIB_DIR/libavutil.56*.dylib $APP_DIR/Contents/Libs/

cp -L $LIB_DIR/libogg.0.dylib $APP_DIR/Contents/Libs/
cp -L $LIB_DIR/libvorbis.0.dylib $APP_DIR/Contents/Libs/
cp -L $LIB_DIR/libvorbisfile.3.dylib $APP_DIR/Contents/Libs/

cp -a $VULKAN_LIB_DIR/libvulkan.*.dylib $APP_DIR/Contents/Libs/
cp -a $VULKAN_LIB_DIR/libMoltenVK.dylib $APP_DIR/Contents/Libs/

cp $DXVK_NATIVE_PATH $APP_DIR/Contents/Libs/

cp $BUILD_DIR/Source/perimeter $APP_DIR/Contents/MacOS/Perimeter

install_name_tool -change "@rpath/libvulkan.1.dylib" "$VULKAN_LIB_DIR/libvulkan.1.dylib" $APP_DIR/Contents/Libs/libdxvk_d3d9.dylib
install_name_tool -change "@rpath/libdxvk_d3d9.dylib" "$APP_DIR/Contents/Libs/libdxvk_d3d9.dylib" $APP_DIR/Contents/MacOS/Perimeter

dylibbundler -x $APP_DIR/Contents/MacOS/Perimeter

chmod 777 $APP_DIR/Contents/Libs/*

install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib "@executable_path/../Libs/libogg.0.dylib" $APP_DIR/Contents/Libs/libvorbis.0.dylib
install_name_tool -change /usr/local/Cellar/libvorbis/1.3.6/lib/libvorbis.0.dylib "@executable_path/../Libs/libvorbis.0.dylib" $APP_DIR/Contents/Libs/libvorbisfile.3.dylib
install_name_tool -change /usr/local/opt/libogg/lib/libogg.0.dylib "@executable_path/../Libs/libogg.0.dylib" $APP_DIR/Contents/Libs/libvorbisfile.3.dylib
#install_name_tool -change "$LIB_DIR/libavutil.56.dylib" "@executable_path/../Libs/libavutil.56.dylib" $APP_DIR/Contents/Libs/libavcodec.58.dylib
#install_name_tool -change "$LIB_DIR/libavutil.56.dylib" "@executable_path/../Libs/libavutil.56.dylib" $APP_DIR/Contents/Libs/libavformat.58.dylib
#install_name_tool -change "$LIB_DIR/libavcodec.58.dylib" "@executable_path/../Libs/libavcodec.58.dylib" $APP_DIR/Contents/Libs/libavformat.58.dylib
install_name_tool -change "/usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib" "@executable_path/../Libs/libSDL2-2.0.0.dylib" $APP_DIR/Contents/Libs/libSDL2_image-2.0.0.dylib
install_name_tool -change "/usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib" "@executable_path/../Libs/libSDL2-2.0.0.dylib" $APP_DIR/Contents/Libs/libSDL2_mixer-2.0.0.dylib
install_name_tool -change "/usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib" "@executable_path/../Libs/libSDL2-2.0.0.dylib" $APP_DIR/Contents/Libs/libSDL2_net-2.0.0.dylib
install_name_tool -change "/usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib" "@executable_path/../Libs/libSDL2-2.0.0.dylib" $APP_DIR/Contents/Libs/libSDL2_net-2.0.0.dylib
install_name_tool -change "$VULKAN_LIB_DIR/libvulkan.1.dylib" "@executable_path/../Libs/libvulkan.1.dylib" "$APP_DIR/Contents/Libs/libdxvk_d3d9.dylib"
install_name_tool -change "$APP_DIR/Contents/Libs/libdxvk_d3d9.dylib" "@executable_path/../Libs/libdxvk_d3d9.dylib" $APP_DIR/Contents/MacOS/Perimeter
