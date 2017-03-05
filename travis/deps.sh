#!/bin/bash -eux
if [[ "$BUILD_TYPE" == "Docs" ]] ; then
  sudo apt-get update -qq
  sudo apt-get install -qq doxygen doxygen-doc doxygen-gui graphviz
  exit 0
fi

case "$TRAVIS_OS_NAME" in
  linux)
    sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 1397BC53640DB551
#    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo add-apt-repository --yes ppa:beineri/opt-qt58-trusty
    sudo add-apt-repository --yes ppa:jonathonf/gcc-6.3
    sudo apt-get update -qq
    sudo apt-get install -qq g++-6 binutils ninja-build gcovr lcov qt58-meta-minimal libasound2-dev

    sudo wget https://downloads.sourceforge.net/project/boost/boost/1.63.0/boost_1_63_0.tar.bz2 -O /opt/boost.tar.bz2
    (cd /opt; sudo tar xaf boost.tar.bz2; sudo mv boost_* boost ; sudo chmod -R a+rwx boost)

    wget https://cmake.org/files/v3.8/cmake-3.8.0-rc1-Linux-x86_64.tar.gz -O cmake-linux.tgz
    tar xaf cmake-linux.tgz
    mv cmake-*-x86_64 cmake
  ;;
  osx)
    # work around a homebrew bug
    set +e
    brew install gnu-tar xz
    ARCHIVE=homebrew-cache.tar.xz
    wget "https://github.com/OSSIA/iscore-sdk/releases/download/6.0-osx/$ARCHIVE" -O "$ARCHIVE"
    gtar xhzf "$ARCHIVE" --directory /usr/local/Cellar
    brew link --force boost cmake ninja qt5

    set -e
  ;;
esac
