#!/bin/bash
(
mkdir max
cd max
cmake -GNinja ~/i-score/API -DCMAKE_BUILD_TYPE=Release -DOSSIA_MAX=1 -DOSSIA_PYTHON=1 -DOSSIA_PD=1 -DOSSIA_QT=0 -DOSSIA_STATIC=1 -DOSSIA_OSX_FAT_LIBRARIES=1 -DOSSIA_OSX_RETROCOMPATIBILITY=1 -DCMAKE_INSTALL_PREFIX=tutu
cmake --build . 
cmake --build . --target install
)

(
mkdir native
cd native
cmake -GNinja ~/i-score/API -DCMAKE_BUILD_TYPE=Release -DOSSIA_C=1 -DOSSIA_CPP=1 -DOSSIA_OSX_FAT_LIBRARIES=1 -DOSSIA_OSX_RETROCOMPATIBILITY=1 -DCMAKE_INSTALL_PREFIX=tutu
cmake --build .
cmake --build . --target install
)

(
mkdir qt
cd qt
cmake -GNinja ~/i-score/API -DCMAKE_BUILD_TYPE=Release -DOSSIA_QT=1 -DOSSIA_QML=1 -DOSSIA_OSX_FAT_LIBRARIES=1 -DOSSIA_OSX_RETROCOMPATIBILITY=1 -DCMAKE_INSTALL_PREFIX=tutu -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt/5.9.1/lib/cmake/Qt5
cmake --build .
cmake --build . --target install
)


