#!/bin/bash

# Simple script to configure and build ossia-max
# Available command optional arguments:
#    release : build in release mode
#    clean_build : remove any previous build folder and start from scratch

set -ex
export LANG=en_US.UTF-8

# install deps
command -v brew > /dev/null 2>&1 || /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
command -v greadlink > /dev/null 2>&1 || brew install coreutils
command -v ninja > /dev/null 2>&1 || brew install ninja
command -v cmake > /dev/null 2>&1 || brew install cmake

OSSIA_BUILD_TYPE=debug

for var in "$@"
do
  if [[ $var = clean ]]; then
    OSSIA_CLEAN_BUILD=1
  elif	[[ $var = release ]]; then
  	OSSIA_BUILD_TYPE=release
  fi
done

SCRIPT_FOLDER=`dirname "$(greadlink -f "$0")"`
REPO_ROOT=${SCRIPT_FOLDER}/../
OSSIA_BUILD_FOLDER=${REPO_ROOT}/build-ossia-max

if [[ -n ${OSSIA_CLEAN_BUILD} ]]
then
  rm -rf ${OSSIA_BUILD_FOLDER}
fi

mkdir -p ${OSSIA_BUILD_FOLDER}
cd ${OSSIA_BUILD_FOLDER}
cmake -GNinja "${REPO_ROOT}" \
  -DCMAKE_BUILD_TYPE=${OSSIA_BUILD_TYPE} \
  -DOSSIA_MAX_INSTALL_FOLDER="${HOME}/Documents/Max 8/Packages/ossia" \
  -DOSSIA_MAX_ONLY=1 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13
ninja 
ninja install