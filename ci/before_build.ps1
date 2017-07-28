git submodule update --init --recursive

if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){
  curl --retry 10 -sO https://cycling74.s3.amazonaws.com/download/max-sdk-7.3.3.zip
  7z x max-sdk-7.3.3.zip -y > nul
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  if ( Test-Path ${env:QTDIR}\bin\ ) {
    set PATH=${env:QTDIR}\bin;%PATH%
  }
}

mkdir build
cd build
cmake -G ${env:CMAKE_GENERATOR_NAME} -DCMAKE_BUILD_TYPE=${env:configuration} ${env:CMAKE_OPTION}  ..


if ($env:APPVEYOR_BUILD_TYPE -eq "max"){
  cd ..
  mkdir build-32bit
  cd build-32bit
  cmake -G ${env:CMAKE_GENERATOR_NAME} -DCMAKE_BUILD_TYPE=${env:configuration} ${env:CMAKE_OPTION}  ..
  cd ../build
}
