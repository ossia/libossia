Set-PSDebug -Trace 1

function CheckLastExitCode {
    param ([int[]]$SuccessCodes = @(0), [scriptblock]$CleanupScript=$null)

    Push-AppveyorArtifact $LogFile

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

cd C:\projects\libossia\build\
$LogFile = C:\projects\libossia\build-${env:APPVEYOR_BUILD_TYPE}.log
cmake --build . > $LogFile
CheckLastExitCode

if ( $env:APPVEYOR_BUILD_TYPE -eq "max"){
  cd C:\projects\libossia\build-32bit
  $LogFile = C:\projects\libossia\build-${env:APPVEYOR_BUILD_TYPE}-32bit.log
  cmake --build . > $LogFile
  CheckLastExitCode
}
