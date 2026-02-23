#!/bin/bash

root_path=$(pwd)
dock_code=$(find . -name "native-docker-build")

cp -vrf ./docker_build.sh "${root_path}/${dock_code}/docker_build.sh" 
cd "${root_path}/${dock_code}"

make build_dev
make dock_dev

