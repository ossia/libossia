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
    export CC=/usr/bin/gcc-6
    export CXX=/usr/bin/g++-6
    export BOOST_ROOT=/opt/boost
#    export VERBOSE=1
    QT_ENV_SCRIPT=$(find /opt -name 'qt*-env.sh')
    source $QT_ENV_SCRIPT
    export LD_LIBRARY_PATH="/usr/lib64:$LD_LIBRARY_PATH"

    case "$BUILD_TYPE" in
      Debug)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOSSIA_STATIC=$OSSIA_STATIC -DOSSIA_TESTING=1 -DOSSIA_EXAMPLES=1 -DOSSIA_CI=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
      ;;
      Release)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOSSIA_STATIC=$OSSIA_STATIC -DOSSIA_TESTING=1 -DOSSIA_EXAMPLES=1 -DOSSIA_CI=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
      ;;
      PdRelease)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" -DOSSIA_STATIC=1 -DOSSIA_TESTING=0 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_QT=0 -DOSSIA_PYTHON=0 -DOSSIA_NO_QT=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null
        echo List TRAVIS_BUILD_DIR content
        ls $TRAVIS_BUILD_DIR
        tar -cf ossia-pd-linux.tar.gz $TRAVIS_BUILD_DIR/ossia-pd-package/ossia
      ;;
      Coverage)
        gem install coveralls-lcov
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=Debug -DOSSIA_TESTING=1 -DOSSIA_COVERAGE=1 -DOSSIA_CI=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
        rm -rf **/*.o
        lcov --compat-libtool --directory .. --capture --output-file coverage.info
        lcov --remove coverage.info '*.moc' '*/moc_*' '*/qrc_*' '*/ui_*' '*/tests/*' '/usr/*' '/opt/*' '*/3rdparty/*' --output-file coverage.info.cleaned

        mv coverage.info.cleaned coverage.info
        coveralls-lcov coverage.info
      ;;
      Docs)
        cd ../Documentation/Doxygen

        doxygen > doxygen.log
        (
            # inspired from generateDocumentationAndDeploy.sh, Jeroen de Bruijn
            git clone -b gh-pages https://git@$GH_REPO_REF
            cd "$GH_REPO_NAME"
            git checkout --orphan dummy
            git branch -D gh-pages
            git checkout --orphan gh-pages

            echo "$(pwd)"

            # Set the push default to simple i.e. push only the current branch.
            git config --global push.default simple
            # Pretend to be an user called Travis CI.
            git config user.name "Travis CI"
            git config user.email "travis@travis-ci.org"

            rm -rf *

            echo "" > .nojekyll
            mv ../html .

            if [ -d "html" ] && [ -f "html/index.html" ]; then
                echo "Commiting..."
                git add --all
                git commit -m "Deploy code docs to GitHub Pages Travis build: ${TRAVIS_BUILD_NUMBER}" -m "Commit: ${TRAVIS_COMMIT}"
                git push --force "https://${GH_REPO_TOKEN}@${GH_REPO_REF}"
            fi


        )
      ;;
    esac
  ;;

  osx)
    export QT_PATH=$(dirname $(dirname $(find /usr/local/Cellar/qt -name Qt5Config.cmake) ) )
    export CXX=clang++
    export CMAKE_PREFIX_PATH="$QT_PATH"

    export CMAKE_BIN=$(which cmake)

    if [ "$BUILD_TYPE" = "PdRelease" ]; then
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=Release \
               -DOSSIA_STATIC=1 \
               -DOSSIA_SANITIZE=1 \
               -DOSSIA_TESTING=0 \
               -DOSSIA_EXAMPLES=0 \
               -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
               -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
               -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
               -DOSSIA_CI=1 \
               -DOSSIA_QT=0 -DOSSIA_NO_QT=1 \
               ..
      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target install > /dev/null
      echo List TRAVIS_BUILD_DIR content
      ls $TRAVIS_BUILD_DIR
      tar -cf ossia-pd-osx.tar.gz $TRAVIS_BUILD_DIR/ossia-pd-package/ossia

    else
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
               -DOSSIA_STATIC=$OSSIA_STATIC \
               -DOSSIA_SANITIZE=1 \
               -DOSSIA_TESTING=1 \
               -DOSSIA_EXAMPLES=1 \
               -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
               -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
               -DOSSIA_CI=1 \
               ..

      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target ExperimentalTest
    fi
  ;;
esac
