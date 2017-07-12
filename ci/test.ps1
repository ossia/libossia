if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  cd C:\projects\libossia\build\
  dir
  cmake --build . --target RUN_TESTS --config %configuration%
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  echo We should test ossia.dll loading into Pd here.
}
