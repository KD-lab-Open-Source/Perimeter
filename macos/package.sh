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

cp -r $TEMPLATE_DIR $APP_DIR

mkdir $APP_DIR/Contents/Frameworks
mkdir $APP_DIR/Contents/Libs

if [[ -n $VULKAN_LIB_VERSION ]]; then
  VULKAN_LIB_DIR="/usr/local/lib"
  DXVK_NATIVE_PATH="$BUILD_DIR/Source/dxvk-prefix/src/dxvk-build/src/d3d9/libdxvk_d3d9.dylib"
  
  rm -f $BUILD_DIR/libdxvk_d3d9.dylib
  cp $DXVK_NATIVE_PATH $BUILD_DIR/libdxvk_d3d9.dylib
fi

cp $BUILD_DIR/Source/perimeter $APP_DIR/Contents/MacOS/Perimeter

if [[ -n $VULKAN_LIB_VERSION ]]; then
  #Fix dxvk before bundling
  install_name_tool -change "@rpath/libvulkan.1.dylib" "$VULKAN_LIB_DIR/libvulkan.1.dylib" $BUILD_DIR/libdxvk_d3d9.dylib
  install_name_tool -change "@rpath/libdxvk_d3d9.dylib" "$BUILD_DIR/libdxvk_d3d9.dylib" $APP_DIR/Contents/MacOS/Perimeter
fi

dylibbundler -x $APP_DIR/Contents/MacOS/Perimeter -b -d $APP_DIR/Contents/Libs

chmod 777 $APP_DIR/Contents/Libs/*

if [[ -n $VULKAN_LIB_VERSION ]]; then
  cp -L /usr/local/lib/libvulkan.$VULKAN_LIB_VERSION.dylib $APP_DIR/Contents/Libs/
fi
