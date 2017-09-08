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

  ls .
  ls install

  mkdir ossia-pd-package
  mkdir ossia-pd-package\ossia
  mkdir ossia-pd-package\ossia\helps
  mkdir ossia-pd-package\ossia\examples

  copy build\OSSIA\ossia-pd\Release\ossia.dll ossia-pd-package\ossia\
  copy OSSIA\ossia-pd\helps\* ossia-pd-package\ossia\helps\
  copy OSSIA\ossia-pd\examples\* ossia-pd-package\ossia\examples\
  # install target fails with error MSB3073, see https://ci.appveyor.com/project/JeanMichalCelerier/libossia/build/job/65o4lytwm9gr74n2
  # cmake --build . --target install
  # 7z a ossia-pd-windows-x86_64.zip %APPVEYOR_BUILD_FOLDER%\ossia-pd-package\*
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){

  cd c:\projects\libossia\build

  cmake --build . --target install


  ls .
  ls install

  mkdir ossia-max-package
  mkdir ossia-max-package\ossia
  mkdir ossia-max-package\ossia\help
  mkdir ossia-max-package\ossia\examples
  mkdir ossia-max-package\ossia\extensions

  copy build\OSSIA\ossia-max\Release\*.mxe64 ossia-max-package\ossia\externals
  copy build-32bit\OSSIA\ossia-max\Release\*.mxe ossia-max-package\ossia\externals
  copy OSSIA\ossia-max\help\* ossia-max-package\ossia\help\
  copy OSSIA\ossia-max\examples\* ossia-max-package\ossia\examples\
}
