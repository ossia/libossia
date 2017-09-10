Set-PSDebug -Trace 1

if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  cd c:\projects\libossia\build

  if ( $env:configuration -eq "Release" ){
    copy OSSIA\Release\ossia.dll Tests\Release\
  } else {
    copy OSSIA\Debug\ossia.dll Tests\Debug\
  }
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  cd c:\projects\libossia\build

  cmake --build . --target install

  ls ../install
  # install target fails with error MSB3073, see https://ci.appveyor.com/project/JeanMichalCelerier/libossia/build/job/65o4lytwm9gr74n2
  # cmake --build . --target install
  # 7z a ossia-pd-windows-x86_64.zip %APPVEYOR_BUILD_FOLDER%\ossia-pd-package\*
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){

  cd c:\projects\libossia\build

  cmake --build . --target install

  ls ../install
}
