#!/bin/bash -eux
if [[ "$BUILD_TYPE" == "Docs" ]] ; then
  sudo apt-get update -qq
  sudo apt-get install -qq doxygen doxygen-doc doxygen-gui graphviz
  exit 0
fi

case "$TRAVIS_OS_NAME" in
  linux)
    sudo wget https://github.com/OSSIA/iscore-sdk/releases/download/sdk7/boost.tar.bz2 -O /opt/boost.tar.bz2 &

    wget https://cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.tar.gz -O cmake-linux.tgz &

    echo 'deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-4.0 main' | sudo tee /etc/apt/sources.list.d/llvm.list
    sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 1397BC53640DB551
    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo add-apt-repository --yes ppa:beineri/opt-qt59-trusty
    sudo apt-get update -qq
    sudo apt-get install -qq --yes --force-yes g++-6 binutils ninja-build gcovr lcov qt59-meta-minimal libasound2-dev clang-4.0 lld-4.0

    wait wget || true

    (cd /opt; sudo tar xaf boost.tar.bz2; sudo mv boost_* boost ; sudo chmod -R a+rwx boost)

    tar xaf cmake-linux.tgz
    mv cmake-*-x86_64 cmake
  ;;
  osx)
    # work around a homebrew bug
    set +e
    brew install gnu-tar xz
    ARCHIVE=homebrew-cache.tar.xz
    wget --quiet "https://github.com/OSSIA/iscore-sdk/releases/download/sdk7/$ARCHIVE" -O "$ARCHIVE"
    gtar xhzf "$ARCHIVE" --directory /usr/local/Cellar
    brew link --force boost cmake ninja qt5

    if [ "$BUILD_TYPE" = "MaxRelease" ]; then
      mkdir -p "$HOME/Documents/Max 7/Packages"
      cd "$HOME/Documents/Max 7/Packages"
      MAXSDKARCHIVE=max-sdk-7.3.3.zip
      wget --quiet "https://cycling74.s3.amazonaws.com/download/$MAXSDKARCHIVE"
      tar xf "$MAXSDKARCHIVE"
    fi

    set -e
  ;;
esac
