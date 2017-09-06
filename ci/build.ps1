Set-PSDebug -Trace 1

msbuild C:\projects\libossia\build\OSSIA.sln /m > C:\projects\libossia\build-${env:APPVEYOR_BUILD_TYPE}.log

if ( $env:APPVEYOR_BUILD_TYPE -eq "max"){
  cmake --build . --target install

  cmake --build . --target clean
  cmake -G ${env:CMAKE_GENERATOR_NAME} -DCMAKE_BUILD_TYPE=${env:CONFIGURATION} ${env:CMAKE_OPTION}  . > C:\projects\libossia\config-${env:APPVEYOR_BUILD_TYPE}-32bit.log
  cmake --build .
  cmake --build . --target install

}
