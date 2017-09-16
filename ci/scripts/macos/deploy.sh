#!/bin/zsh
mkdir ossia-python
cp max/*.so ossia-python/

mkdir ossia-max
cp -rf max/tutu/ossia-max-package/ossia ossia-max/

mkdir ossia-pd
cp -rf max/tutu/ossia-pd-package/ossia ossia-pd/

mkdir ossia-qml
cp -rf qt/tutu/Ossia ossia-qml/

mkdir ossia-native
cp -rf native/tutu/* ossia-native/

cp -rf ~/i-score/API/ossia/ossia-unity3d .
cp ossia-native/lib/libossia.dylib ossia-unity3d/ossia.bundle

rm -rf **/*.qmlc
rm -rf **/*.meta
rm -rf **/.DS_STORE

zip -r ossia-python-macos.zip ossia-python 
zip -r ossia-max-macos.zip ossia-max
zip -r ossia-pd-macos.zip ossia-pd
zip -r ossia-qml-macos.zip ossia-qml
zip -r libossia-native-macos.zip ossia-native/*
zip -r ossia-unity3d-macos.zip ossia-unity3d
