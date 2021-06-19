#!/bin/sh
set -e

export SSH_DIR=/noexist
if [ ! -z $CLEAN ]; then
  rm ./dockcross/dockcross-*-perimeter
fi
if [ ! -f ./dockcross/dockcross-windows-shared-x86-perimeter ]; then
  docker build -t dockcross-windows-shared-x86-perimeter -f ./dockcross/windows-shared-x86.Dockefile ./dockcross
  docker run dockcross-windows-shared-x86-perimeter > ./dockcross/dockcross-windows-shared-x86-perimeter
  chmod a+x ./dockcross/dockcross-windows-shared-x86-perimeter
fi
if [ ! -f ./dockcross/dockcross-windows-shared-x64-perimeter ]; then
  docker build -t dockcross-windows-shared-x64-perimeter -f ./dockcross/windows-shared-x64.Dockefile ./dockcross
  docker run dockcross-windows-shared-x64-perimeter > ./dockcross/dockcross-windows-shared-x64-perimeter
  chmod a+x ./dockcross/dockcross-windows-shared-x64-perimeter
fi
if [ ! -f ./dockcross/dockcross-windows-static-x86-perimeter ]; then
  docker build -t dockcross-windows-static-x86-perimeter -f ./dockcross/windows-static-x86.Dockefile ./dockcross
  docker run dockcross-windows-static-x86-perimeter > ./dockcross/dockcross-windows-static-x86-perimeter
  chmod a+x ./dockcross/dockcross-windows-static-x86-perimeter
fi
if [ ! -f ./dockcross/dockcross-windows-static-x64-perimeter ]; then
  docker build -t dockcross-windows-static-x64-perimeter -f ./dockcross/windows-static-x64.Dockefile ./dockcross
  docker run dockcross-windows-static-x64-perimeter > ./dockcross/dockcross-windows-static-x64-perimeter
  chmod a+x ./dockcross/dockcross-windows-static-x64-perimeter
fi

echo "Building under windows shared x86"
./dockcross/dockcross-windows-shared-x86-perimeter ./dockcross/cmake-mingw.sh $@
#echo "Building under windows shared x64"
#./dockcross/dockcross-windows-shared-x64-perimeter ./dockcross/cmake-mingw.sh $@
#echo "Building under windows static x86"
#./dockcross/dockcross-windows-static-x86-perimeter ./dockcross/cmake-mingw.sh $@
#echo "Building under windows static x86_64"
#./dockcross/dockcross-windows-static-x64-perimeter ./dockcross/cmake-mingw.sh $@
