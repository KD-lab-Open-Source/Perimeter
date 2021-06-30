#!/bin/sh
set -e

DOCKER_ROOT=./docker
export SSH_DIR=/noexist

if [ ! -z $CLEAN ]; then
  rm $DOCKER_ROOT/dockcross-*-perimeter
fi
if [ ! -f $DOCKER_ROOT/dockcross-x86-perimeter ]; then
  docker build -t dockcross-x86-perimeter -f $DOCKER_ROOT/dockcross-x86.Dockefile $DOCKER_ROOT
  docker run dockcross-x86-perimeter > $DOCKER_ROOT/dockcross-x86-perimeter
  chmod a+x $DOCKER_ROOT/dockcross-x86-perimeter
fi
if [[ ! -z $BUILD_64 ]] && [[ ! -f $DOCKER_ROOT/dockcross-x64-perimeter ]]; then
  docker build -t dockcross-x64-perimeter -f $DOCKER_ROOT/dockcross-x64.Dockefile $DOCKER_ROOT
  docker run dockcross-x64-perimeter > $DOCKER_ROOT/dockcross-x64-perimeter
  chmod a+x $DOCKER_ROOT/dockcross-x64-perimeter
fi

echo "Building under windows x86"
$DOCKER_ROOT/dockcross-x86-perimeter $DOCKER_ROOT/dockcross-cmake.sh $@
if [[ ! -z $BUILD_64 ]]; then
  echo "Building under windows x64"
  $DOCKER_ROOT/dockcross-x64-perimeter $DOCKER_ROOT/dockcross-cmake.sh $@
fi
