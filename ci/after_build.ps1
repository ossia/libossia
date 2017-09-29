Set-PSDebug -Trace 1

function CheckLastExitCode {
    param ([int[]]$SuccessCodes = @(0), [scriptblock]$CleanupScript=$null)

    Push-AppveyorArtifact "$LogFile"
    Push-AppveyorArtifact "C:/projects/libossia/build/CMakeFiles/CMakeOutput.log"
    Push-AppveyorArtifact "C:/projects/libossia/build/CMakeFiles/CMakeError.log"

    if ($SuccessCodes -notcontains $LastExitCode) {
        if ($CleanupScript) {
            "Executing cleanup script: $CleanupScript"
            &$CleanupScript
        }
        $msg = @"
EXE RETURNED EXIT CODE $LastExitCode
CALLSTACK:$(Get-PSCallStack | Out-String)
"@
        throw $msg
    }
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){
  cd c:\projects\libossia\build

  if ( $env:configuration -eq "Release" ){
    mkdir c:\projects\libossia\build\Test\Release
    copy c:\projects\libossia\build\OSSIA\Release\ossia.dll c:\projects\libossia\build\Tests\Release\
  } else {
    mkdir c:\projects\libossia\build\Test\Debug
    copy c:\projects\libossia\build\OSSIA\Debug\ossia.dll c:\projects\libossia\build\Tests\Debug\
  }
} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "Release" ){

  cd c:\projects\libossia\build
  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-${env:APPVEYOR_BUILD_TYPE}-win64.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode
  cd ${env:APPVEYOR_BUILD_FOLDER}\install
  ls

  7z a ${env:APPVEYOR_BUILD_FOLDER}\libossia-native-win64.zip .

  cd c:\projects\libossia\build-32bit
  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-${env:APPVEYOR_BUILD_TYPE}-win32.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode
  cd ${env:APPVEYOR_BUILD_FOLDER}\install-32bit
  ls

  7z a ${env:APPVEYOR_BUILD_FOLDER}\libossia-native-win32.zip .

  # make unity3d package
  mkdir ${env:APPVEYOR_BUILD_FOLDER}\unity3d
  mkdir ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets
  mkdir ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets\Plugins\x86
  mkdir ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets\Plugins\x86_64
  mkdir ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets\ossia
  xcopy ${env:APPVEYOR_BUILD_FOLDER}\OSSIA\ossia-unity3d\* ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets\ossia\ /s /e
  mv ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets\ossia\README.md ${env:APPVEYOR_BUILD_FOLDER}\unity3d\
  cp ${env:APPVEYOR_BUILD_FOLDER}\install\bin\ossia.dll ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets\Plugins\x86_64
  cp ${env:APPVEYOR_BUILD_FOLDER}\install-32bit\bin\ossia.dll ${env:APPVEYOR_BUILD_FOLDER}\unity3d\Assets\Plugins\x86

  cd ${env:APPVEYOR_BUILD_FOLDER}\unity3d\
  7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-unity3d-win.zip .

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  cd c:\projects\libossia\build

  $LogFile = "C:\projects\libossia\install-pd.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd ${env:APPVEYOR_BUILD_FOLDER}\install\ossia-pd-package\
  ls .

  7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-pd-win32.zip .

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "qml" ){
  cd c:\projects\libossia\build

  $LogFile = "C:\projects\libossia\install-qml.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd ${env:APPVEYOR_BUILD_FOLDER}\install\
  ls

  7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-qml-win64.zip .

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "python" ){
  cd c:\projects\libossia\build
  dir

  if ( ${env:platform} -eq x64){
    7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-${env:python}-win64.zip "ossia_python.so"
  } else {
    7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-${env:python}-win32.zip "ossia_python.so"
  }


} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){

  cd c:\projects\libossia\build

  $LogFile = "C:\projects\libossia\install-max.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd c:\projects\libossia\build-32bit

  $LogFile = "C:\projects\libossia\install-max-32bit.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd ${env:APPVEYOR_BUILD_FOLDER}\install\ossia-max-package\
  ls

  7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-max-win.zip .
}
