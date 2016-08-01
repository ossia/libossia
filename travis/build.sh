#!/bin/bash
# Note : to make the tests work under travis, they have to be changed in order not to require QApplication but only QCoreApplication

case "$TRAVIS_OS_NAME" in
  linux)
  export CMAKE_BIN=$(readlink -f "$(find cmake/bin -name cmake -type f )")
  ;;
  osx)
  export CMAKE_BIN=$(which cmake)
  ;;
esac
export CTEST_OUTPUT_ON_FAILURE=1

mkdir build
cd build

case "$TRAVIS_OS_NAME" in
  linux)
    export CC=gcc-6
    export CXX=g++-6
    export BOOST_ROOT=/opt/boost
    QT_ENV_SCRIPT=$(find /opt -name 'qt*-env.sh')
    source $QT_ENV_SCRIPT
    export LD_LIBRARY_PATH="/usr/lib64:$LD_LIBRARY_PATH"
    case "$BUILD_TYPE" in
      Debug)
        $CMAKE_BIN -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOSSIA_STATIC=$OSSIA_STATIC -DOSSIA_SANITIZE=1 -DOSSIA_TESTING=1 -DOSSIA_EXAMPLES=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
      ;;
      Release)
        $CMAKE_BIN -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOSSIA_STATIC=$OSSIA_STATIC -DOSSIA_SANITIZE=1 -DOSSIA_TESTING=1 -DOSSIA_EXAMPLES=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
      ;;
      Coverage)
        gem install coveralls-lcov
        $CMAKE_BIN -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=Debug -DOSSIA_TESTING=1 -DOSSIA_COVERAGE=1 ..
        $CMAKE_BIN --build . --target ossia_coverage
        mv coverage.info.cleaned coverage.info
        coveralls-lcov coverage.info
      ;;
      Docs)
        cd ../Documentation/Doxygen
        doxygen
      ;;
  ;;

  osx)
    export QT_PATH=$(dirname $(dirname $(find /usr/local/Cellar/qt5 -name Qt5Config.cmake) ) )
    export CXX=clang++
    export CMAKE_PREFIX_PATH="$QT_PATH"

    export CMAKE_BIN=$(which cmake)
    $CMAKE_BIN -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
               -DOSSIA_STATIC=$OSSIA_STATIC \
               -DOSSIA_SANITIZE=1 \
               -DOSSIA_TESTING=1 \
               -DOSSIA_EXAMPLES=1 \
               -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
               -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
               ..

    $CMAKE_BIN --build . -- -j2
    $CMAKE_BIN --build . --target ExperimentalTest
  ;;
esac
