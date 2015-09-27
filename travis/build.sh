#!/bin/bash
# Note : to make the tests work under travis, they have to be changed in order not to require QApplication but only QCoreApplication
#    - LD_LIBRARY_PATH=/usr/lib64 make ExperimentalTest


mkdir build
cd build
export CMAKE_COMMON_FLAGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
export CTEST_OUTPUT_ON_FAILURE=1

case "$TRAVIS_OS_NAME" in
  linux)
    source /opt/qt55/bin/qt55-env.sh
    /usr/local/bin/cmake $CMAKE_COMMON_FLAGS ..
    make -j2
    make ExperimentalTest
  ;;
  osx)
    cmake -DCMAKE_PREFIX_PATH="/usr/local/Cellar/qt5/5.5.0/lib/cmake;$(pwd)/../Jamoma/share/cmake" $CMAKE_COMMON_FLAGS ..

    make -j2
    make ExperimentalTest
  ;;
esac
