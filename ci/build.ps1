Set-PSDebug -Trace 1

cd C:\projects\libossia\build\
cmake --build . > C:\projects\libossia\build-${env:APPVEYOR_BUILD_TYPE}.log

if ( $env:APPVEYOR_BUILD_TYPE -eq "max"){
  cd C:\projects\libossia\build-32bit
  cmake --build . > C:\projects\libossia\build-${env:APPVEYOR_BUILD_TYPE}-32bit.log
}
