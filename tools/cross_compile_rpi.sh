#!/bin/bash

cmake -S libossia -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=/libossia//cmake/toolchain/arm-linux-gnueabihf.cmake -DCMAKE_INSTALL_PREFIX=/ossia-install
cmake --build build
cmake --build build --target install
