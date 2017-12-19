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
  cd C:\projects\libossia\build\
  dir
  cmake --build . --target RUN_TESTS --config %configuration%
}

if ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ){
  [Environment]::SetEnvironmentVariable("Path",$env:Path + ";${env:APPVEYOR_BUILD_FOLDER}/build/OSSIA/ossia-pd/pd/bin/","Process")            

  cd ${env:APPVEYOR_BUILD_FOLDER}\build

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\test-pd.log"
  cmake --build . --config "${env:configuration}" --target RUN_TESTS > "$LogFile"
  CheckLastExitCode
}
