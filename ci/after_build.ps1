if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  copy OSSIA\%configuration%\ossia.dll Tests\%configuration%\
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  mkdir ossia-pd-package\ossia
  mkdir ossia-pd-package\ossia\helps
  mkdir ossia-pd-package\ossia\examples

  copy OSSIA\ossia-pd\%configuration%\ossia.dll ossia-pd-package\ossia\
  copy %APPVEYOR_BUILD_FOLDER%\OSSIA\ossia-pd\helps\* ossia-pd-package\ossia\helps\
  copy %APPVEYOR_BUILD_FOLDER%\OSSIA\ossia-pd\examples\* ossia-pd-package\ossia\examples\
  # install target fails with error MSB3073, see https://ci.appveyor.com/project/JeanMichalCelerier/libossia/build/job/65o4lytwm9gr74n2
  # cmake --build . --target install
  # 7z a ossia-pd-windows-x86_64.zip %APPVEYOR_BUILD_FOLDER%\ossia-pd-package\*
}
