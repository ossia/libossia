cd C:\projects\libossia\build\
msbuild C:\projects\libossia\build\OSSIA.sln /m

if ( $env:APPVEYOR_BUILD_TYPE -eq "max"){
  cd C:\projects\libossia\build-32bit
  msbuild C:\projects\libossia\build-32bit\OSSIA.sln /m
}
