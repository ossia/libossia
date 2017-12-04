#!/bin/bash

# This script should be execute inside a docker

export PATH=/opt/gcc-6/bin:/opt/cmake/bin:$PATH
export LD_LIBRARY_PATH=/opt/gcc-6/lib

git clone --depth=1 https://github.com/OSSIA/libossia

(
  mkdir -p build; cd build
  cmake ../libossia \
      -DCMAKE_C_COMPILER=/opt/gcc-6/bin/gcc \
      -DCMAKE_CXX_COMPILER=/opt/gcc-6/bin/g++ \
      -DBOOST_ROOT=/opt/boost_1_62_0 \
      -DCMAKE_SHARED_LINKER_FLAGS="-static-libstdc++" \
      -DOSSIA_STATIC=1
  make -j8
)
