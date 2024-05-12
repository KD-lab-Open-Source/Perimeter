#!/bin/bash
set -e

DOCKER_ROOT=./docker/dockcross
export SSH_DIR=/noexist

if [ ! -z $CLEAN ]; then
  rm -vf ${DOCKER_ROOT}/dockcross-*-perimeter
fi

if [[ -z "${ARCH}" ]]; then
  DOCKCROSS_ARCH=x64
else
  DOCKCROSS_ARCH=${ARCH}
fi
echo "Building under windows ${DOCKCROSS_ARCH}"
DOCKCROSS_SCRIPT=$DOCKER_ROOT/dockcross-x86-perimeter

if [[ -n $BUILD ]] || [[ ! -f ${DOCKCROSS_SCRIPT} ]]; then
  docker build -t dockcross-${DOCKCROSS_ARCH}-perimeter -f $DOCKER_ROOT/${DOCKCROSS_ARCH}_Dockerfile .
  docker run dockcross-${DOCKCROSS_ARCH}-perimeter > ${DOCKCROSS_SCRIPT}
  chmod a+x ${DOCKCROSS_SCRIPT}
fi

${DOCKCROSS_SCRIPT} $DOCKER_ROOT/dockcross-cmake.sh $@

