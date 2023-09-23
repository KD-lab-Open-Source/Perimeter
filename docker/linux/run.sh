#!/usr/bin/env bash
set -e

DOCKER_ROOT="./docker/linux"
if [[ ! -d $DOCKER_ROOT ]]; then
  echo "Run this from project root"
  exit 1
fi
DOCKER_IMAGE="docker-linux-perimeter"

docker build -t $DOCKER_IMAGE -f $DOCKER_ROOT/Dockerfile .

mkdir -p "${PWD}/build/docker-linux"

echo "Running $DOCKER_IMAGE"
docker run \
 --rm \
 --interactive \
 --tty \
 --volume "${PWD}/":/work \
 --env WORK_DIR=/work \
 --env LIBS_ROOT=/opt/out \
 $DOCKER_IMAGE \
 /bin/bash /work/docker/linux/builder $@
