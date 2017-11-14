#!/bin/bash -x
# Note : to make the tests work under travis, they have to be changed in order not to require QApplication but only QCoreApplication

case "$TRAVIS_OS_NAME" in
  linux)
    export CMAKE_BIN=$(readlink -f "$(find cmake/bin -name cmake -type f )")
    if [[ "$PYTHON_VERSION" == "2.7" ]]; then
      export PYTHON_BIN=$(which python)
    else
      export PYTHON_BIN=$(which python3)
    fi
  ;;
  osx)
    export CMAKE_BIN=$(which cmake)
    if [[ "$PYTHON_VERSION" == "2.7" ]]; then
      export PYTHON_BIN=/usr/local/bin/python2
    else
      export PYTHON_BIN=/usr/local/bin/python3
    fi
  ;;
esac

export CTEST_OUTPUT_ON_FAILURE=1

mkdir -p ${ARTIFACTS_DIR}

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
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" \
          -DCMAKE_CXX_COMPILER="$CXX" \
          -DBOOST_ROOT="$BOOST_ROOT" \
          -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
          -DOSSIA_STATIC=$OSSIA_STATIC \
          -DOSSIA_TESTING=1 \
          -DOSSIA_EXAMPLES=1 \
          -DOSSIA_CI=1 \
          -DOSSIA_QT=1 \
          -DOSSIA_PD=0 \
          ..

        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest

      ;;
      Release)
        OSSIA_UNITY=1
        if [[ "$OSSIA_STATIC" == "1" ]]; then
          OSSIA_UNITY=0
        fi

        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" \
          -DCMAKE_CXX_COMPILER="$CXX" \
          -DBOOST_ROOT="$BOOST_ROOT" \
          -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/install" \
          -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
          -DOSSIA_C=1 \
          -DOSSIA_CPP=1 \
          -DOSSIA_UNITY3D=$OSSIA_UNITY \
          -DOSSIA_STATIC=$OSSIA_STATIC \
          -DOSSIA_TESTING=0 \
          -DOSSIA_EXAMPLES=0 \
          -DOSSIA_PD=0 \
          -DOSSIA_CI=1 \
          -DOSSIA_QT=1 ..

        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target ExperimentalTest
        $CMAKE_BIN --build . --target install


        if [[ "$OSSIA_STATIC" == "1" ]]; then
          cd $TRAVIS_BUILD_DIR/install
          tar -czf ${ARTIFACTS_DIR}/libossia-native-linux_x86_64-static.tar.gz *
        else
          # make unity3d package
          cd $TRAVIS_BUILD_DIR/install/unity3d/
          tar -czf ${ARTIFACTS_DIR}/ossia-unity3d-linux_x86_64.tar.gz *

          cd $TRAVIS_BUILD_DIR/install
          rm -rf unity3d
          tar -czf ${ARTIFACTS_DIR}/libossia-native-linux_x86_64.tar.gz *
        fi

      ;;
      PdRelease)

        pushd /tmp
        git clone ${TRAVIS_BUILD_DIR} --recursive
        tar -czf ${ARTIFACTS_DIR}/libossia-source.tar.gz --exclude .git libossia
        rm -rf libossia
        popd

        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" \
                   -DCMAKE_CXX_COMPILER="$CXX" \
                   -DBOOST_ROOT="$BOOST_ROOT" \
                   -DCMAKE_BUILD_TYPE=Release \
                   -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
                   -DOSSIA_STATIC=1 \
                   -DOSSIA_TESTING=0 \
                   -DOSSIA_EXAMPLES=0 \
                   -DOSSIA_CI=1 \
                   -DOSSIA_QT=0 \
                   -DOSSIA_NO_QT=1 \
                   -DOSSIA_PYTHON=0 \
                   ..
        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null

        cd $TRAVIS_BUILD_DIR/ossia-pd-package
        tar -czf ${ARTIFACTS_DIR}/ossia-pd-linux_x86_64.tar.gz ossia

        $TRAVIS_BUILD_DIR/ci/push_deken.sh
      ;;
      RpiPdRelease)

        #setup some environment variable to help CMAKE to find libraries
        export RPI_ROOT_PATH=/tmp/rpi/root
        export PKG_CONFIG_SYSROOT_DIR=$RPI_ROOT_PATH
        export PKG_CONFIG_LIBDIR=${RPI_ROOT_PATH}/usr/lib/pkgconfig:${RPI_ROOT_PATH}/usr/share/pkgconfig:${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/pkgconfig/

        $CMAKE_BIN -DCMAKE_TOOLCHAIN_FILE="$PWD/../CMake/toolchain/arm-linux-gnueabihf.cmake" \
                   -DBOOST_ROOT="/usr/include/boost" \
                   -DCMAKE_BUILD_TYPE=Release \
                   -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
                   -DOSSIA_STATIC=1 \
                   -DOSSIA_TESTING=0 \
                   -DOSSIA_EXAMPLES=0 \
                   -DOSSIA_CI=1 \
                   -DOSSIA_QT=0 \
                   -DOSSIA_PYTHON=0 \
                   -DOSSIA_NO_QT=1 \
                   -DALSA_INCLUDE_DIR=${RPI_ROOT_PATH}/usr/include \
                   -DALSA_LIBRARY=${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/libasound.so \
                   ..

        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null

        cd $TRAVIS_BUILD_DIR/ossia-pd-package
        tar -czf ${ARTIFACTS_DIR}/ossia-pd-linux_arm.tar.gz ossia

        $TRAVIS_BUILD_DIR/ci/push_deken.sh
      ;;
      RpiPythonRelease)
        #setup some environment variable to help CMAKE to find libraries
        export RPI_ROOT_PATH=/tmp/rpi/root
        export PKG_CONFIG_SYSROOT_DIR=$RPI_ROOT_PATH
        export PKG_CONFIG_LIBDIR=${RPI_ROOT_PATH}/usr/lib/pkgconfig:${RPI_ROOT_PATH}/usr/share/pkgconfig:${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/pkgconfig/
        # _version.py is not valid in a non-git folder
        # When making a wheel, we write the git tag which it has been build from
        # request the version
        WHEEL_TAG_VERSION=$(echo -e "import sys\nsys.path.append('${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/')\nfrom pyossia._version import get_versions\nget_versions()['version']" | ${PYTHON_BIN})
        echo "#! /usr/bin/env python
        # -*- coding: utf-8 -*-

        def get_versions():
            return {'version':'${WHEEL_TAG_VERSION}'}" > ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/pyossia/_version.py
        $CMAKE_BIN -DCMAKE_TOOLCHAIN_FILE="$PWD/../CMake/toolchain/arm-linux-gnueabihf.cmake" \
                   -DBOOST_ROOT="/usr/include/boost" \
                   -DPYTHON_INCLUDE_DIR=${RPI_ROOT_PATH}/usr/include/python${PYTHON_VERSION} \
                   -DCMAKE_BUILD_TYPE=Release \
                   -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
                   -DPYTHON_EXECUTABLE=${PYTHON_BIN} \
                   -DOSSIA_STATIC=1 \
                   -DOSSIA_TESTING=0 \
                   -DOSSIA_EXAMPLES=0 \
                   -DOSSIA_CI=1 \
                   -DOSSIA_QT=0 \
                   -DOSSIA_PYTHON=1 \
                   -DOSSIA_NO_QT=1 \
                   -DOSSIA_PD=0 \
                   -DALSA_INCLUDE_DIR=${RPI_ROOT_PATH}/usr/include \
                   -DALSA_LIBRARY=${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/libasound.so \
                   ..

        $CMAKE_BIN --build . -- -j2

        if [[ "x${TRAVIS_TAG}" != "x" ]]; then
          ${PYTHON_BIN} -m twine upload -u ${PyPiUser} -p {PyPiWord} ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl || true
        fi
        cp ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl ${ARTIFACTS_DIR}/
      ;;
      RpiRelease)
        #setup some environment variable to help CMAKE to find libraries
        export RPI_ROOT_PATH=/tmp/rpi/root
        export PKG_CONFIG_SYSROOT_DIR=$RPI_ROOT_PATH
        export PKG_CONFIG_LIBDIR=${RPI_ROOT_PATH}/usr/lib/pkgconfig:${RPI_ROOT_PATH}/usr/share/pkgconfig:${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/pkgconfig/

        $CMAKE_BIN -DCMAKE_TOOLCHAIN_FILE="$PWD/../CMake/toolchain/arm-linux-gnueabihf.cmake" \
                   -DBOOST_ROOT="/usr/include/boost" \
                   -DOSSIA_PD=0 \
                   -DCMAKE_BUILD_TYPE=Release \
                   -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/install" \
                   -DOSSIA_STATIC=$OSSIA_STATIC \
                   -DOSSIA_TESTING=0 \
                   -DOSSIA_EXAMPLES=0 \
                   -DOSSIA_CI=1 \
                   -DOSSIA_QT=0 \
                   -DOSSIA_PYTHON=0 \
                   -DOSSIA_C=1 \
                   -DOSSIA_CPP=1 \
                   -DALSA_INCLUDE_DIR=${RPI_ROOT_PATH}/usr/include \
                   -DALSA_LIBRARY=${RPI_ROOT_PATH}/usr/lib/arm-linux-gnueabihf/libasound.so \
                   ..

        $CMAKE_BIN --build . -- -j2
        $CMAKE_BIN --build . --target install > /dev/null

        cd $TRAVIS_BUILD_DIR/install
        if [[ "$OSSIA_STATIC" ==  "1" ]]; then
          tar -czf ${ARTIFACTS_DIR}/libossia-native-linux_arm-static.tar.gz *
        else
          tar -czf ${ARTIFACTS_DIR}/libossia-native-linux_arm.tar.gz *
        fi
      ;;
      python_manylinux)
        # _version.py is not valid in a non-git folder
        # When making a wheel, we write the git tag which it has been build from
        # request the version
        WHEEL_TAG_VERSION=$(echo -e "import sys\nsys.path.append('${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/')\nfrom pyossia._version import get_versions\nget_versions()['version']" | ${PYTHON_BIN})
        echo "#! /usr/bin/env python
        # -*- coding: utf-8 -*-

        def get_versions():
            return {'version':'${WHEEL_TAG_VERSION}'}" > ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/pyossia/_version.py

        docker run --rm -v `pwd`:/ $DOCKER_IMAGE $PRE_CMD ci/build-wheels.sh

        ls wheelhouse/
        cp wheelhouse/*.whl ${ARTIFACTS_DIR}/
        
      ;;
      python)
        # _version.py is not valid in a non-git folder
        # When making a wheel, we write the git tag which it has been build from
        # request the version
        WHEEL_TAG_VERSION=$(echo -e "import sys\nsys.path.append('${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/')\nfrom pyossia._version import get_versions\nget_versions()['version']" | ${PYTHON_BIN})
        echo "#! /usr/bin/env python
        # -*- coding: utf-8 -*-
        
        def get_versions():
            return {'version':'${WHEEL_TAG_VERSION}'}" > ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/pyossia/_version.py
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" \
          -DCMAKE_CXX_COMPILER="$CXX" \
          -DBOOST_ROOT="$BOOST_ROOT" \
          -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/ossia-python" \
          -DPYTHON_EXECUTABLE=${PYTHON_BIN} \
          -DOSSIA_STATIC=1 \
          -DOSSIA_TESTING=0 \
          -DOSSIA_EXAMPLES=0 \
          -DOSSIA_CI=1 \
          -DOSSIA_PD=0 \
          -DOSSIA_QT=0 \
          -DOSSIA_QML=0 \
          -DOSSIA_PYTHON=1 ..

        $CMAKE_BIN --build . -- -j2
        # now we just want to install the wheel and run the tests
        ${PYTHON_BIN} -m pip install --user ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl
        ${PYTHON_BIN} ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/tests/test.py

        if [[ "x${TRAVIS_TAG}" != "x" ]]; then
          ${PYTHON_BIN} -m twine upload -u ${PyPiUser} -p ${PyPiWord} ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl || true
        fi

        ${PYTHON_BIN} -m pip install --user ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl
        ${PYTHON_BIN} ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/tests/test.py

        cp ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl ${ARTIFACTS_DIR}/

      ;;
      qml)
        $CMAKE_BIN -DCMAKE_C_COMPILER="$CC" \
          -DCMAKE_CXX_COMPILER="$CXX" \
          -DBOOST_ROOT="$BOOST_ROOT" \
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
        tar -czf ${ARTIFACTS_DIR}/ossia-qml-linux_x86_64.tar.gz Ossia
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
      pushd $TRAVIS_BUILD_DIR/ossia-pd-package/
      tar -czf ${ARTIFACTS_DIR}/ossia-pd-osx.tar.gz ossia
      popd

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
      pushd $TRAVIS_BUILD_DIR/ossia-max-package/
      tar -czf $TRAVIS_BUILD_DIR/ossia-max-package/ossia-max-osx.tar.gz ossia
      popd

    elif [[ "$BUILD_TYPE" == "python" ]]; then
      # _version.py is not valid in a non-git folder
      # When making a wheel, we write the git tag which it has been build from
      # request the version
      WHEEL_TAG_VERSION=$(echo -e "import sys\nsys.path.append('${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/')\nfrom pyossia._version import get_versions\nget_versions()['version']" | ${PYTHON_BIN})
      echo "#! /usr/bin/env python
      # -*- coding: utf-8 -*-

      def get_versions():
          return {'version':'${WHEEL_TAG_VERSION}'}" > ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/pyossia/_version.py
      $CMAKE_BIN -DCMAKE_BUILD_TYPE=Release \
                 -DOSSIA_STATIC=1 \
                 -DOSSIA_SANITIZE=1 \
                 -DOSSIA_TESTING=0 \
                 -DOSSIA_EXAMPLES=0 \
                 -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" \
                 -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR" \
                 -DPYTHON_EXECUTABLE=${PYTHON_BIN} \
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
      # now we just want to install the wheel and run the tests
      ${PYTHON_BIN} -m pip install --user ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl
      ${PYTHON_BIN} ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/tests/test.py

      if [[ "x${TRAVIS_TAG}" != "x" ]]; then
          ${PYTHON_BIN} -m twine upload -u ${PyPiUser} -p ${PyPiWord} ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl || true
          mv ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl ${ARTIFACTS_DIR}/
      fi
      cp ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl ${ARTIFACTS_DIR}/
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
                 -DOSSIA_QT=1 \
                 -DOSSIA_PYTHON=0 \
                 -DOSSIA_PD=0 \
                 -DOSSIA_MAX=0 \
                 -DOSSIA_QML=1 \
                 -DOSSIA_OSX_RETROCOMPATIBILITY=1 \
                 ..
      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target install > /dev/null

      cd "$TRAVIS_BUILD_DIR/ossia-qml"
      tar -czf ${ARTIFACTS_DIR}/ossia-qml-osx.tar.gz Ossia

    else
      OSSIA_UNITY=1
      if [[ "$OSSIA_STATIC" == "1" ]]; then
        OSSIA_UNITY=0
      fi

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
               -DOSSIA_UNITY3D=$OSSIA_UNITY \
               -DOSSIA_OSX_RETROCOMPATIBILITY=1 \
               -DCMAKE_INSTALL_PREFIX=$TRAVIS_BUILD_DIR/install \
               ..

      $CMAKE_BIN --build . -- -j2
      $CMAKE_BIN --build . --target ExperimentalTest
      $CMAKE_BIN --build . --target install

      if [[ "$BUILD_TYPE" == "Release" ]]; then
        if [[ "$OSSIA_STATIC" == "1" ]]; then
          cd $TRAVIS_BUILD_DIR/install
          tar -czf ${ARTIFACTS_DIR}/libossia-native-macos-static.tar.gz *
        else
          cd $TRAVIS_BUILD_DIR/install/unity3d
          tar -czf ${ARTIFACTS_DIR}/ossia-unity3d-macos.tar.gz *

          cd $TRAVIS_BUILD_DIR/install
          rm -rf unity3d
          tar -czf ${ARTIFACTS_DIR}/libossia-native-macos.tar.gz *
        fi
      fi
    fi
  ;;
esac
