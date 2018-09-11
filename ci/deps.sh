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
      sudo wget -nv https://github.com/OSSIA/score-sdk/releases/download/sdk7/boost.tar.bz2 -O /opt/boost.tar.bz2 &

      wget -nv https://cmake.org/files/v3.11/cmake-3.11.3-Linux-x86_64.tar.gz -O cmake-linux.tgz &

      echo 'deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-6.0 main' | sudo tee /etc/apt/sources.list.d/llvm.list
      sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 1397BC53640DB551
      sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
      sudo add-apt-repository --yes ppa:beineri/opt-qt-5.10.1-trusty
      sudo apt-get update -qq
      sudo apt-get install -qq --yes --force-yes g++-7 binutils ninja-build gcovr lcov qt510-meta-minimal libasound2-dev clang-6.0 lld-6.0 portaudio19-dev

      wait wget || true

      (cd /opt; sudo tar xaf boost.tar.bz2; sudo mv boost_* boost ; sudo chmod -R a+rwx boost)

      tar xaf cmake-linux.tgz
      mv cmake-*-x86_64 cmake
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
          sudo apt install -qq python python-dev python-pip
          sudo pip install twine
        else
          sudo add-apt-repository --yes ppa:jonathonf/python-3.6
          sudo apt-get update -qq
          sudo apt install -qq python3 python3-dev python3-pip
          sudo pip3 install twine
        fi
      fi
    fi
  ;;
  osx)
    # work around a homebrew bug
    set +e
    # try to force a ruby update
    # brew need Ruby 2.3, but OSX comes with 2.0
    # HOMEBREW_NO_AUTO_UPDATE=1 brew install ruby
    HOMEBREW_NO_AUTO_UPDATE=1 brew install gnu-tar xz
    ARCHIVE=homebrew-cache.txz
    wget -nv "https://github.com/OSSIA/score-sdk/releases/download/sdk9/$ARCHIVE" -O "$ARCHIVE"
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
