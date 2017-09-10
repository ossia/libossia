Set-PSDebug -Trace 1

if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  cd c:\projects\libossia\build

  if ( $env:configuration -eq "Release" ){
    mkdir c:\projects\libossia\build\Test\Release
    copy c:\projects\libossia\build\OSSIA\Release\ossia.dll c:\projects\libossia\build\Tests\Release\
  } else {
    mkdir c:\projects\libossia\build\Test\Debug
    copy c:\projects\libossia\build\OSSIA\Debug\ossia.dll c:\projects\libossia\build\Tests\Debug\
  }
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  cd c:\projects\libossia\build

  cmake --build . --target install > C:\projects\libossia\install-pd.log

  ls ../install
  ls ../install/ossia-pd-package/*
  # install target fails with error MSB3073, see https://ci.appveyor.com/project/JeanMichalCelerier/libossia/build/job/65o4lytwm9gr74n2
  # cmake --build . --target install
  # 7z a ossia-pd-windows-x86_64.zip %APPVEYOR_BUILD_FOLDER%\ossia-pd-package\*
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){

  cd c:\projects\libossia\build

  cmake --build . --target install > C:\projects\libossia\install-max.log

  cd c:\projects\libossia\build-32bit

  cmake --build . --target install > C:\projects\libossia\install-max-32bit.log

  ls ../install
  ls ../install/ossia-max-package/*
}
