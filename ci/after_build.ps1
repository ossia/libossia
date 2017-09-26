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

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-${env:APPVEYOR_BUILD_TYPE}-${env:platform}.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  ls ../install
  ls ../install/ossia-pd-package/*

  7z a ossia-windows.zip %APPVEYOR_BUILD_FOLDER%\install\ossia\*

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  cd c:\projects\libossia\build

  $LogFile = "C:\projects\libossia\install-pd.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  ls ../install
  ls ../install/ossia-pd-package/*

  7z a ossia-pd-windows.zip %APPVEYOR_BUILD_FOLDER%\install\ossia-pd-package\*

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "qml" ){
  cd c:\projects\libossia\build

  $LogFile = "C:\projects\libossia\install-qml.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  ls ../install
  7z a ossia-pd-windows-x86_64.zip %APPVEYOR_BUILD_FOLDER%\install\*

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "python" ){
  cd c:\projects\libossia\build

  7z a ..\ossia-${env:python}-windows.zip "ossia_python.so"

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){

  cd c:\projects\libossia\build

  $LogFile = "C:\projects\libossia\install-max.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd c:\projects\libossia\build-32bit

  $LogFile = "C:\projects\libossia\install-max-32bit.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  ls ../install
  ls ../install/ossia-max-package/*

  7z a ossia-max-windows.zip %APPVEYOR_BUILD_FOLDER%\install\ossia-max-package\*
}
