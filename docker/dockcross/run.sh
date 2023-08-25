#!/bin/bash
set -e

DOCKER_ROOT=./docker/dockcross
export SSH_DIR=/noexist

if [[ -z $BUILD_32 ]] && [[ -z $BUILD_64 ]]; then
  BUILD_64=1;
fi

if [ ! -z $CLEAN ]; then
  rm $DOCKER_ROOT/dockcross-*-perimeter
fi

if [[ -n $BUILD_DOCKER ]] || [[ ! -f $DOCKER_ROOT/dockcross-x64-perimeter ]]; then
  docker build -t dockcross-x64-perimeter -f $DOCKER_ROOT/dockcross-x64.Dockerfile $DOCKER_ROOT
  docker run dockcross-x64-perimeter > $DOCKER_ROOT/dockcross-x64-perimeter
  chmod a+x $DOCKER_ROOT/dockcross-x64-perimeter
fi

echo "Building under windows x64"
$DOCKER_ROOT/dockcross-x64-perimeter $DOCKER_ROOT/dockcross-cmake.sh $@
  
