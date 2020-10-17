#!/bin/bash -eux
if [[ "$BUILD_TYPE" == "Docs" ]] ; then
  sudo apt-get update -qq
  sudo apt-get install -qq doxygen doxygen-doc doxygen-gui graphviz
  exit 0
fi

case "$TRAVIS_OS_NAME" in
  linux)
    if [[ "$BUILD_TYPE" == "RpiDocker" ]]; then
      sudo apt-get install -qq binfmt-support qemu qemu-user-static
      set +e
      sudo update-binfmts --enable qemu-arm
      docker pull iscore/iscore-rpi-sdk:latest
      set -e
    else
      wget -nv https://cmake.org/files/v3.18/cmake-3.18.3-Linux-x86_64.tar.gz -O cmake-linux.tgz &

      echo 'deb http://apt.llvm.org/focal/ llvm-toolchain-focal-10 main' | sudo tee /etc/apt/sources.list.d/llvm.list
      sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 1397BC53640DB551 15CF4D18AF4F7421
      sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
      sudo add-apt-repository --yes ppa:beineri/opt-qt-5.15.0-focal
      sudo apt-get update -qq
      sudo apt-get install -qq --yes --force-yes \
          g++-9 binutils ninja-build \
          clang-10 lld-10 \
          qt515base qt515declarative qt515svg qt515quickcontrols2 qt515websockets qt515serialport \
          libasound2-dev portaudio19-dev \
          mesa-common-dev libgl1-mesa-dev \
          libbluetooth-dev \
          libavahi-client-dev

      wait wget || true

      tar xaf cmake-linux.tgz
      mv cmake-*-x86_64 cmake-latest
    fi

    if [[ "$BUILD_TYPE" == "Coverage" ]]; then
      gem install coveralls-lcov

      sudo apt-get install -qq python3 python3-pip
      sudo pip3 install --upgrade wheel setuptools
      sudo pip3 install git+https://github.com/rpgillespie6/fastcov.git

      # For some reason gcov seems to be not available anymore...
      wget -nv https://github.com/ossia/sdk/releases/download/sdk14/gcov
      file ./gcov
      chmod +x ./gcov
      sudo cp ./gcov /usr/bin/gcov
      sudo cp ./gcov /usr/bin/gcov-9

      gcov-9 --version
    fi

    shopt -s nocasematch # case insensitive comparison in Bash
    if [[ "$BUILD_TYPE" == Rpi* ]]; then
        # install dependencies
        wget -nv https://www.dropbox.com/s/lwchd3va49sd43p/gcc-8.2.0-rpi.tar.bz2
        tar xf gcc-8.2.0-rpi.tar.bz2
        sudo mv cross-pi-gcc-8.2.0/ /opt/
        sudo ln -s /opt/cross-pi-gcc-8.2.0 /opt/cross-pi-gcc

        sudo ln -s /usr/include/arm-linux-gnueabihf/sys /usr/include/sys
        sudo ln -s /usr/include/arm-linux-gnueabihf/bits /usr/include/bits
        sudo ln -s /usr/include/arm-linux-gnueabihf/gnu /usr/include/gnu
        sudo ln -s /usr/include/arm-linux-gnueabihf/asm /usr/include/asm
        sudo ln -s /usr/lib/arm-linux-gnueabihf/crti.o /usr/lib/crti.o
        sudo ln -s /usr/lib/arm-linux-gnueabihf/crt1.o /usr/lib/crt1.o
        sudo ln -s /usr/lib/arm-linux-gnueabihf/crtn.o /usr/lib/crtn.o
    fi

    if [[ $BUILD_TYPE == *python* ]]; then
      if [[ "$BUILD_TYPE" == "python_manylinux" ]]; then
        if [[ "${PRE_CMD:-linux64}" == "linux32" ]]; then
          docker pull quay.io/pypa/manylinux1_i686
        else
          docker pull quay.io/pypa/manylinux1_x86_64
        fi
        wget https://bootstrap.pypa.io/get-pip.py
        # Install setuptools (need for build wheel)
        for PYBIN in /opt/python/*/bin/; do
            "${PYBIN}/python" get-pip.py
            "${PYBIN}/pip" install setuptools
        done
      else
        if [[ "$PYTHON_VERSION" == "2.7" ]]; then
          sudo apt-get update -qq
          sudo apt-get install -qq python python-dev python-pip
          sudo pip install twine wheel
        else
          sudo apt-get update -qq
          sudo apt-get install -qq python3 python3-dev python3-pip
          sudo pip3 install twine wheel
        fi
      fi
    fi
  ;;
  osx)
    # Setup codesigning
    # Thanks https://www.update.rocks/blog/osx-signing-with-travis/
    (
      set +x
      KEY_CHAIN=build.keychain

      openssl aes-256-cbc -K $encrypted_5c96fe262983_key -iv $encrypted_5c96fe262983_iv -in "$TRAVIS_BUILD_DIR/ci/ossia-cert.p12.enc" -out ossia-cert.p12 -d

      security create-keychain -p travis $KEY_CHAIN
      security default-keychain -s $KEY_CHAIN
      security unlock-keychain -p travis $KEY_CHAIN
      security import ossia-cert.p12 -k $KEY_CHAIN -P $MAC_CODESIGN_PASSWORD -T /usr/bin/codesign;
      security set-key-partition-list -S apple-tool:,apple: -s -k travis $KEY_CHAIN

      rm -rf *.p12
    )

    # work around a homebrew bug
    set +e
    # try to force a ruby update
    # brew need Ruby 2.3, but OSX comes with 2.0
    # HOMEBREW_NO_AUTO_UPDATE=1 brew install ruby
    HOMEBREW_NO_AUTO_UPDATE=1 brew install gnu-tar xz
    ARCHIVE=homebrew-cache.txz
    wget -nv "https://github.com/ossia/score-sdk/releases/download/sdk9/$ARCHIVE" -O "$ARCHIVE"
    gtar xhzf "$ARCHIVE" --directory /usr/local/Cellar
    brew link --force boost cmake qt5 #ninja
    brew install portaudio

    if [[ "x$BUILD_TYPE" == "xpython" || $BUILD_TYPE == *Pd* ]]; then
      if [[ "x$PYTHON_VERSION" == x2.* ]]; then
        brew install python2
        pip install wheel --user
        pip install twine --user
      else
        brew install python3
        pip3 install wheel --user
        pip3 install twine --user
      fi
    fi

    set -e
  ;;
esac
