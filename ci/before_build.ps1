Set-PSDebug -Trace 1

cd  C:\projects\libossia
git submodule update --init --recursive

if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){
  curl --retry 10 -sO https://cycling74.s3.amazonaws.com/download/max-sdk-7.3.3.zip
  7z x max-sdk-7.3.3.zip -y > nul
}

mkdir build
cd build

if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  if ( Test-Path ${env:QTDIR}\bin\ ) {
    set $env:PATH=${env:QTDIR}\bin;${env:PATH};
  }

  if ( $env:configuration -eq "Release" ){
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DOSSIA_PD=0 -DOSSIA_CI=1 -DOSSIA_TESTING=1 -DBOOST_ROOT="${env:BOOST_ROOT}" -DCMAKE_PREFIX_PATH="${env:QTDIR}\lib\cmake\Qt5" c:\projects\libossia
  } else {
    cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug   -DOSSIA_PD=0 -DOSSIA_CI=1 -DOSSIA_TESTING=1 -DBOOST_ROOT="${env:BOOST_ROOT}" -DCMAKE_PREFIX_PATH="${env:QTDIR}\lib\cmake\Qt5" c:\projects\libossia
  }
  # cmake --build .

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){
  cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release -DOSSIA_MAX=1 -DMAXSDK_MAINPATH="${env:APPVEYOR_BUILD_FOLDER}\max-sdk-7.3.3\source" -DOSSIA_PD=0 -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="${env:BOOST_ROOT}"
 # cmake --build .
 # cmake --build . --target install

# now build 32 bit version
  cd ..
  mkdir build-32bit
  cd build-32bit

  cmake -G "Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Release -DOSSIA_MAX=1 -DMAXSDK_MAINPATH="${env:APPVEYOR_BUILD_FOLDER}\max-sdk-7.3.3\source" -DOSSIA_PD=0 -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="${env:BOOST_ROOT}" c:\projects\libossia
  cmake --build .
  cmake --build . --target install
} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  cmake -G "Visual Studio 15 2017" -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_TYPE=Release -DOSSIA_PD=1 -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="${env:BOOST_ROOT}" c:\projects\libossia
  # cmake --build .
  # cmake --build . --target install
}

exit

# Testing Debug
#      CMAKE_OPTION: -DCMAKE_BUILD_TYPE=Debug -DOSSIA_PD=0 -DOSSIA_CI=1 -DOSSIA_TESTING=1 -DBOOST_ROOT="%BOOST_ROOT%" -DCMAKE_PREFIX_PATH="%QTDIR%\lib\cmake\Qt5"

# Testing Release
#      CMAKE_OPTION: -DCMAKE_BUILD_TYPE=Release -DOSSIA_PD=0 -DOSSIA_CI=1 -DOSSIA_TESTING=1 -DBOOST_ROOT="%BOOST_ROOT%" -DCMAKE_PREFIX_PATH="%QTDIR%\lib\cmake\Qt5"

# max
#      CMAKE_OPTION: -DCMAKE_BUILD_TYPE=Release -DOSSIA_MAX=1 -DMAXSDK_MAINPATH="%APPVEYOR_BUILD_FOLDER%\max-sdk-7.3.3\source" -DOSSIA_PD=0 -DCMAKE_INSTALL_PREFIX="%APPVEYOR_BUILD_FOLDER%" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="%BOOST_ROOT%"

# Pd
#      CMAKE_OPTION: -DCMAKE_BUILD_TYPE=Release -DOSSIA_PD=1 -DCMAKE_INSTALL_PREFIX="%APPVEYOR_BUILD_FOLDER%" -DOSSIA_STATIC=1 -DOSSIA_QT=0 -DOSSIA_NO_QT=1 -DOSSIA_EXAMPLES=0 -DOSSIA_CI=1 -DOSSIA_TESTING=0 -DBOOST_ROOT="%BOOST_ROOT%"


# ok: cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release c:\projects\libossia
cmake -G "${env:CMAKE_GENERATOR_NAME}" -DCMAKE_BUILD_TYPE=${env:configuration} c:\projects\libossia
# Write-Host cmake -G "${env:CMAKE_GENERATOR_NAME}" ${env:CMAKE_OPTION}  .. ">" C:\projects\libossia\config-${env:APPVEYOR_BUILD_TYPE}.log
# cmake -G "${env:CMAKE_GENERATOR_NAME}" -DCMAKE_BUILD_TYPE=${env:configuration} ${env:CMAKE_OPTION}  C:\projects\libossia  > C:\projects\libossia\config-${env:APPVEYOR_BUILD_TYPE}.log
