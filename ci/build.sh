#!/bin/bash -x
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

tar -czf ossia-src-unix.tar.gz .

mkdir build
cd build

case "$TRAVIS_OS_NAME" in
  linux)
    if [[ -f /usr/bin/gcc-7 ]] ; then
      export CC=/usr/bin/gcc-7
      export CXX=/usr/bin/g++-7
    else
      export CC=/usr/bin/gcc-6
      export CXX=/usr/bin/g++-6
    fi
    export BOOST_ROOT=/opt/boost
#    export VERBOSE=1
    QT_ENV_SCRIPT=$(find /opt -name 'qt*-env.sh')
    source $QT_ENV_SCRIPT
    export LD_LIBRARY_PATH="/usr/lib64:$LD_LIBRARY_PATH"

    case "$BUILD_TYPE" in
      Debug)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOSSIA_STATIC=$OSSIA_STATIC -DOSSIA_TESTING=1 -DOSSIA_EXAMPLES=1 -DOSSIA_CI=1 -DOSSIA_QT=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
      ;;
      Release)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" \
          -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/install" \
          -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
          -DOSSIA_C=1 \
          -DOSSIA_CPP=1 \
          -DOSSIA_UNITY3D=1 \
          -DOSSIA_STATIC=$OSSIA_STATIC \
          -DOSSIA_TESTING=1 \
          -DOSSIA_EXAMPLES=1 \
          -DOSSIA_CI=1 \
          -DOSSIA_QT=1 ..

        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
        $CMAKE_BIN --build . --target install


        if [[ "$OSSIA_STATIC" == "1" ]]; then
          cd $TRAVIS_BUILD_DIR/install
          tar -czf $TRAVIS_BUILD_DIR/ossia-native-linux_x86_64-static.tar.gz .

          # make unity3d package
          mkdir $TRAVIS_BUILD_DIR/unity3d
          mkdir $TRAVIS_BUILD_DIR/unity3d/Assets
          mkdir $TRAVIS_BUILD_DIR/unity3d/Assets/Plugins
          mkdir $TRAVIS_BUILD_DIR/unity3d/Assets/ossia
          cp $TRAVIS_BUILD_DIR/OSSIA/ossia-unity3d/* $TRAVIS_BUILD_DIR/unity3d/Assets/ossia/
          mv $TRAVIS_BUILD_DIR/unity3d/Assets/ossia/README.md $TRAVIS_BUILD_DIR/unity3d/
          cp $TRAVIS_BUILD_DIR/install/lib/libossia.dylib $TRAVIS_BUILD_DIR/unity3d/Assets/Plugins

          cd $TRAVIS_BUILD_DIR/unity3d/
          tar -czf $TRAVIS_BUILD_DIR/ossia-unity3d-linux_x86_64.tar.gz .

        else
          cd $TRAVIS_BUILD_DIR/install
          tar -czf $TRAVIS_BUILD_DIR/ossia-native-linux_x86_64.tar.gz .
        fi

      ;;
      PdRelease)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" -DOSSIA_STATIC=1 -DOSSIA_TESTING=0 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_PYTHON=0 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null

        cd $TRAVIS_BUILD_DIR/ossia-pd-package
        tar -czf $TRAVIS_BUILD_DIR/ossia-pd-linux_x86_64.tar.gz ossia

        $TRAVIS_BUILD_DIR/ci/push_deken.sh
      ;;
      RpiPdRelease)
        #setup some environment variable to help CMAKE to find libraries
        export RPI_ROOT_PATH=/tmp/rpi/root
        export PKG_CONFIG_SYSROOT_DIR=$RPI_ROOT_PATH
        export PKG_CONFIG_LIBDIR=${RPI_ROOT_PATH}/usr/lib/pkgconfig:${RPI_ROOT_PATH}/usr/share/pkgconfig:${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/pkgconfig/

        $CMAKE_BIN -DCMAKE_TOOLCHAIN_FILE="$PWD/../CMake/toolchain/arm-linux-gnueabihf.cmake" -DBOOST_ROOT="/usr/include/boost" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" -DOSSIA_STATIC=1 -DOSSIA_TESTING=0 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_QT=0 -DOSSIA_PYTHON=0 -DOSSIA_NO_QT=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null

        cd $TRAVIS_BUILD_DIR/ossia-pd-package
        tar -czf $TRAVIS_BUILD_DIR/ossia-pd-linux_arm.tar.gz ossia

        $TRAVIS_BUILD_DIR/ci/push_deken.sh
      ;;
      RpiRelease)
        #setup some environment variable to help CMAKE to find libraries
        export RPI_ROOT_PATH=/tmp/rpi/root
        export PKG_CONFIG_SYSROOT_DIR=$RPI_ROOT_PATH
        export PKG_CONFIG_LIBDIR=${RPI_ROOT_PATH}/usr/lib/pkgconfig:${RPI_ROOT_PATH}/usr/share/pkgconfig:${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/pkgconfig/

        $CMAKE_BIN -DCMAKE_TOOLCHAIN_FILE="$PWD/../CMake/toolchain/arm-linux-gnueabihf.cmake" -DBOOST_ROOT="/usr/include/boost" -DOSSIA_PD=0 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/install" -DOSSIA_STATIC=$OSSIA_STATIC -DOSSIA_TESTING=0 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_QT=0 -DOSSIA_PYTHON=0 -DOSSIA_C=1 -DOSSIA_CPP=1 ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null

        cd $TRAVIS_BUILD_DIR/install
        if [[ "$OSSIA_STATIC" ==  "1" ]]; then
          tar -czf $TRAVIS_BUILD_DIR/ossia-native-linux_arm-static.tar.gz .
        else
          tar -czf $TRAVIS_BUILD_DIR/ossia-native-linux_arm.tar.gz .
        fi
      ;;
      python)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" \
             -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/ossia-python" \
             -DPYTHON_EXECUTABLE=`which python${PYTHON_VERSION}` \
             -DOSSIA_STATIC=1 \
             -DOSSIA_TESTING=0 \
             -DOSSIA_EXAMPLES=0 \
             -DOSSIA_CI=1 \
             -DOSSIA_PD=0 \
             -DOSSIA_QT=0 \
             -DOSSIA_QML=0 \
             -DOSSIA_PYTHON=1 ..

        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null
        ls
         if [[ "$PYTHON_VERSION" == "2.7" ]]; then
          pip wheel ../OSSIA/ossia-python/
          pip install twine
          #pip install -ve ../OSSIA/ossia-python/
          #python ../OSSIA/ossia-python/tests/test_.py
        elif [[ "$PYTHON_VERSION" == "3.5" ]]; then
          python3.5 -m pip wheel ../OSSIA/ossia-python/
          pip3 install twine
          #python3.5 -m pip install -ve ../OSSIA/ossia-python/
          #python3.5 ../OSSIA/ossia-python/tests/test_.py
        elif [[ "$PYTHON_VERSION" == "3.6" ]]; then
          python3.6 -m pip wheel ../OSSIA/ossia-python/
          pip3 install twine
          #python3.6 -m pip install -ve ../OSSIA/ossia-python/
          #python3 ../OSSIA/ossia-python/tests/test_.py
        fi
      ;;
      qml)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/ossia-qml" \
          -DOSSIA_STATIC=0 \
          -DOSSIA_TESTING=0 \
          -DOSSIA_EXAMPLES=0 \
          -DOSSIA_CI=1 \
          -DOSSIA_PD=0 \
          -DOSSIA_QT=1 \
          -DOSSIA_QML=1 \
          -DOSSIA_PYTHON=0 ..

        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null


        cd "$TRAVIS_BUILD_DIR/ossia-qml"
        tar -czf $TRAVIS_BUILD_DIR/ossia-qml-linux_x86_64.tar.gz Ossia
      ;;
      RpiDocker)
        echo "Building for Rpi in Docker"
        docker run -it  -v $TRAVIS_BUILD_DIR/ci/docker.sh:/docker.sh iscore/iscore-rpi-sdk /bin/bash /docker.sh
      ;;
      Coverage)
        gem install coveralls-lcov
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" -DBOOST_ROOT="$BOOST_ROOT" -DCMAKE_BUILD_TYPE=Debug -DOSSIA_TESTING=1 -DOSSIA_COVERAGE=1 -DOSSIA_CI=1 -DOSSIA_QT=1 ..
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

    if [[ "$BUILD_TYPE" == "PdRelease" ]]; then
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=Release \
               -DOSSIA_STATIC=1 \
               -DOSSIA_SANITIZE=1 \
               -DOSSIA_TESTING=0 \
               -DOSSIA_EXAMPLES=0 \
               -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
               -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
               -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
               -DOSSIA_CI=1 \
               -DOSSIA_QT=0 \
               -DOSSIA_NO_QT=1 \
               -DOSSIA_PYTHON=0 \
               -DOSSIA_PD=1 \
               -DOSSIA_MAX=0 \
               -DOSSIA_OSX_RETROCOMPATIBILITY=1 \
               ..
      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target install > /dev/null
      echo List TRAVIS_BUILD_DIR content
      cd $TRAVIS_BUILD_DIR
      ls
      tar -czf ossia-pd-osx.tar.gz $TRAVIS_BUILD_DIR/ossia-pd-package/ossia

      $TRAVIS_BUILD_DIR/ci/push_deken.sh

    elif [[ "$BUILD_TYPE" == "MaxRelease" ]]; then
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=Release \
               -DOSSIA_STATIC=1 \
               -DOSSIA_SANITIZE=1 \
               -DOSSIA_TESTING=0 \
               -DOSSIA_EXAMPLES=0 \
               -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
               -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
               -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
               -DOSSIA_CI=1 \
               -DOSSIA_QT=0 \
               -DOSSIA_NO_QT=1 \
               -DOSSIA_PYTHON=0 \
               -DOSSIA_PD=0 \
               -DOSSIA_MAX=1 \
               -DOSSIA_OSX_RETROCOMPATIBILITY=1 \
               ..
      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target install > /dev/null
      echo List TRAVIS_BUILD_DIR content
      cd $TRAVIS_BUILD_DIR
      ls
      tar -czf ossia-max-osx.tar.gz $TRAVIS_BUILD_DIR/ossia-max-package/ossia

    elif [[ "$BUILD_TYPE" == "python" ]]; then
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=Release \
                 -DOSSIA_STATIC=1 \
                 -DOSSIA_SANITIZE=1 \
                 -DOSSIA_TESTING=0 \
                 -DOSSIA_EXAMPLES=0 \
                 -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
                 -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
                 -DPYTHON_EXECUTABLE=/usr/local/bin/python${PYTHON_VERSION} \
                 -DPYTHON_LIBRARY=/usr/local/opt/python/Frameworks/Python.framework/Versions/${python}/lib/libpython${python}.dylib \
                 -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
                 -DOSSIA_CI=1 \
                 -DOSSIA_QT=0 \
                 -DOSSIA_PYTHON=1 \
                 -DOSSIA_PD=0 \
                 -DOSSIA_MAX=0 \
                 -DOSSIA_OSX_RETROCOMPATIBILITY=1 \
                 ..

      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target install > /dev/null
      ls
       if [[ "$PYTHON_VERSION" == "2.7" ]]; then
        pip wheel ../OSSIA/ossia-python/
        pip install twine
        #pip install -ve ../OSSIA/ossia-python/
        #python ../OSSIA/ossia-python/tests/test_.py
      elif [[ "$PYTHON_VERSION" == "3.5" ]]; then
        python3.5 -m pip wheel ../OSSIA/ossia-python/
        pip3 install twine
        #python3.5 -m pip install -ve ../OSSIA/ossia-python/
        #python3.5 ../OSSIA/ossia-python/tests/test_.py
      elif [[ "$PYTHON_VERSION" == "3.6" ]]; then
        python3.6 -m pip wheel ../OSSIA/ossia-python/
        pip3 install twine
        #python3.6 -m pip install -ve ../OSSIA/ossia-python/
        #python3 ../OSSIA/ossia-python/tests/test_.py
      fi

    elif [[ "$BUILD_TYPE" == "qml" ]]; then
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=Release \
                 -DOSSIA_STATIC=0 \
                 -DOSSIA_SANITIZE=1 \
                 -DOSSIA_TESTING=0 \
                 -DOSSIA_EXAMPLES=0 \
                 -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
                 -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR"/ossia-qml \
                 -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
                 -DOSSIA_CI=1 \
                 -DOSSIA_QT=0 \
                 -DOSSIA_PYTHON=0 \
                 -DOSSIA_PD=0 \
                 -DOSSIA_MAX=0 \
                 -DOSSIA_QML=1 \
                 -DOSSIA_OSX_RETROCOMPATIBILITY=1 \
                 ..
      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target install > /dev/null

      cd "$TRAVIS_BUILD_DIR/ossia-qml"
      tar -czf $TRAVIS_BUILD_DIR/ossia-qml-osx.tar.gz Ossia

    else
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
               -DOSSIA_STATIC=$OSSIA_STATIC \
               -DOSSIA_SANITIZE=1 \
               -DOSSIA_TESTING=1 \
               -DOSSIA_EXAMPLES=1 \
               -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
               -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
               -DOSSIA_CI=1 \
               -DOSSIA_QT=1 \
               -DOSSIA_C=1 \
               -DOSSIA_CPP=1 \
               -DOSSIA_UNITY3D=1 \
               -DOSSIA_OSX_RETROCOMPATIBILITY=1 \
               -DCMAKE_INSTALL_PREFIX=$TRAVIS_BUILD_DIR/install
               ..

      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target ExperimentalTest
      $CMAKE_BIN --build . --target install

      if [[ "$BUILD_TYPE" == "Release" ]]; then
        if [[ "$OSSIA_STATIC" == "1" ]]; then
          # make unity3d package
          mkdir $TRAVIS_BUILD_DIR/unity3d
          cp $TRAVIS_BUILD_DIR/OSSIA/ossia-unity3d/* $TRAVIS_BUILD_DIR/unity3d
          cp $TRAVIS_BUILD_DIR/install/lib/libossia.dylib $TRAVIS_BUILD_DIR/unity3d/ossia.bundle

          cd $TRAVIS_BUILD_DIR/unity3d/
          tar -czf $TRAVIS_BUILD_DIR/ossia-unity3d-macos.tar.gz .

          cd $TRAVIS_BUILD_DIR/install
          tar -czf $TRAVIS_BUILD_DIR/ossia-native-macos-static.tar.gz .
        else
          cd $TRAVIS_BUILD_DIR/install
          tar -czf $TRAVIS_BUILD_DIR/ossia-native-macos.tar.gz .
        fi
      fi
    fi
  ;;
esac
