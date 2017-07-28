msbuild C:\projects\libossia\build\OSSIA.sln /m

if ( $env:APPVEYOR_BUILD_TYPE -eq "max"){
  msbuild C:\projects\libossia\build-32bit\OSSIA.sln /m
}
