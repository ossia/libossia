if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  copy OSSIA\%configuration%\ossia.dll Tests\%configuration%\
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  cmake --build . --target install
  # 7z a ossia-pd-windows-x86_64.zip %APPVEYOR_BUILD_FOLDER%\ossia-pd-package\*
}
