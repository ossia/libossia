#!/bin/bash -eux

git submodule init

case "$TRAVIS_OS_NAME" in
  linux)
    sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 1397BC53640DB551
    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo add-apt-repository --yes ppa:beineri/opt-qt57-trusty
    sudo apt-get update -qq
    sudo apt-get install -qq g++-6 ninja-build gcovr lcov qt57-meta-minimal libasound2-dev

    sudo wget https://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.tar.bz2 -O /opt/boost.tar.bz2
    (cd /opt; sudo tar xaf boost.tar.bz2; sudo mv boost_* boost ; sudo chmod -R a+rwx boost)

    wget https://cmake.org/files/v3.6/cmake-3.6.0-rc1-Linux-x86_64.tar.gz -O cmake-linux.tgz
    tar xaf cmake-linux.tgz
    mv cmake-*-x86_64 cmake
  ;;
  osx)
    # work around a homebrew bug
    set +e

    brew install wget gnu-tar
    wget https://github.com/OSSIA/iscore-sdk/releases/download/2.0-OSX/homebrew-cache.tar.gz -O homebrew-cache.tar.gz
    gtar xhzf homebrew-cache.tar.gz --directory /usr/local/Cellar
    brew link --force boost cmake ninja qt5 wget

    set -e
  ;;
esac
