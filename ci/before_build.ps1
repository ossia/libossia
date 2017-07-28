git submodule update --init --recursive

if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){
  curl --retry 10 -sO https://cycling74.s3.amazonaws.com/download/max-sdk-7.3.3.zip
  7z x max-sdk-7.3.3.zip -y > nul
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  if (exist "%QTDIR%\bin\"){
    set PATH=%QTDIR%\bin;%PATH%
  }
}

mkdir build
cd build
cmake -G "%CMAKE_GENERATOR_NAME%" -DCMAKE_BUILD_TYPE=%configuration% %CMAKE_OPTION%  ..


if ($env:APPVEYOR_BUILD_TYPE -eq "max"){
  cd ..
  mkdir build-32bit
  cd build-32bit
  cmake -G "%CMAKE_GENERATOR_NAME_32BIT%" -DCMAKE_BUILD_TYPE=%configuration% %CMAKE_OPTION%  ..
  cd ../build
}
