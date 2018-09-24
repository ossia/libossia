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

cd  C:\projects\libossia
# git submodule update --init --recursive
# if ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){
#   appveyor DownloadFile https://cycling74.s3.amazonaws.com/download/max-sdk-7.3.3.zip
#   7z x max-sdk-7.3.3.zip -y
# }

mkdir build
cd build

$CommonFlags32 = "-G""Visual Studio 15 2017""","-Thost=x64","-DOSSIA_EDITOR=0","-DOSSIA_DATAFLOW=0","-DCMAKE_BUILD_TYPE=Release","-DOSSIA_CI=1","-DOSSIA_TESTING=0","-DOSSIA_EXAMPLES=0","-DOSSIA_PD=0","-DOSSIA_PYTHON=0","-DOSSIA_QT=0","-DOSSIA_PROTOCOL_AUDIO=0","-DCMAKE_INSTALL_PREFIX=""${env:APPVEYOR_BUILD_FOLDER}/install"""
$CommonFlags64 = "-G""Visual Studio 15 2017 Win64""","-Thost=x64","-DOSSIA_EDITOR=0","-DOSSIA_DATAFLOW=0","-DCMAKE_BUILD_TYPE=Release","-DOSSIA_CI=1","-DOSSIA_TESTING=0","-DOSSIA_EXAMPLES=0","-DOSSIA_PD=0","-DOSSIA_PYTHON=0","-DOSSIA_QT=0","-DOSSIA_PROTOCOL_AUDIO=0","-DCMAKE_INSTALL_PREFIX=""${env:APPVEYOR_BUILD_FOLDER}/install"""
if ( $env:APPVEYOR_BUILD_TYPE -eq "testing" ){

  if ( Test-Path ${env:QTDIR}\bin\ ) {
    set $env:PATH=${env:QTDIR}\bin;${env:PATH};
  }

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\config-${env:APPVEYOR_BUILD_TYPE}-${env:configuration}.log"
  
  cmake -G "Visual Studio 15 2017 Win64" -T host=x64 -DOSSIA_C=1 -DOSSIA_CPP=1 -DOSSIA_PD=0 -DOSSIA_CI=1 -DOSSIA_TESTING=1 -DOSSIA_EDITOR=1 -DOSSIA_DATAFLOW=1 -DOSSIA_QT=1 -DOSSIA_QML=1 -DCMAKE_PREFIX_PATH="${env:QTDIR}\lib\cmake\Qt5" -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" c:\projects\libossia > $LogFile

  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "Release" ) {
  if ( Test-Path ${env:QTDIR}\bin\ ) {
    set $env:PATH=${env:QTDIR}\bin;${env:PATH};
  }

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\config-${env:APPVEYOR_BUILD_TYPE}-win64.log"
  cmake $CommonFlags64 -DOSSIA_C=1 -DOSSIA_CPP=1 -DOSSIA_UNITY3D=0 c:\projects\libossia > $LogFile
  CheckLastExitCode

  # now configure 32 bit version
  cd ..
  mkdir build-32bit
  cd build-32bit

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\config-${env:APPVEYOR_BUILD_TYPE}-win32.log"
  cmake $CommonFlags32 -DOSSIA_C=1 -DOSSIA_CPP=1 -DOSSIA_UNITY3D=0 c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "ossia-unity3d" ) {
  if ( Test-Path ${env:QTDIR}\bin\ ) {
    set $env:PATH=${env:QTDIR}\bin;${env:PATH};
  }

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\config-${env:APPVEYOR_BUILD_TYPE}-win64.log"
  cmake $CommonFlags64 -DOSSIA_UNITY3D_ONLY=1 c:\projects\libossia > $LogFile
  CheckLastExitCode

  # now configure 32 bit version
  cd ..
  mkdir build-32bit
  cd build-32bit

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\config-${env:APPVEYOR_BUILD_TYPE}-win32.log"
  cmake $CommonFlags32 -DOSSIA_UNITY3D_ONLY=1 c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "max" ) {
  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\config-${env:APPVEYOR_BUILD_TYPE}-win64.log"
  cmake $CommonFlags64 -DOSSIA_MAX=1 -DOSSIA_STATIC=1 -DOSSIA_PROTOCOL_MIDI=OFF c:\projects\libossia > $LogFile
  CheckLastExitCode

  # now configure 32 bit version
  cd ..
  mkdir build-32bit
  cd build-32bit

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\config-${env:APPVEYOR_BUILD_TYPE}-win32.log"
  cmake $CommonFlags32 -DOSSIA_MAX=1 -DOSSIA_STATIC=1 -DOSSIA_PROTOCOL_MIDI=OFF -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "ossia-cpp" ) {
  $LogFile = "c:\projects\libossia\configure-opp.log"
  cmake -G"Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install" -DOSSIA_STATIC=0 -DOSSIA_CPP_ONLY=1 c:\projects\libossia > $LogFile
  CheckLastExitCode

  # now configure 32 bit version
  cd ..
  mkdir build-32bit
  cd build-32bit

  $LogFile = "c:\projects\libossia\configure-opp-32bit.log"
  cmake -G"Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX="${env:APPVEYOR_BUILD_FOLDER}/install-32bit" -DOSSIA_STATIC=0 -DOSSIA_CPP_ONLY=1 c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "pd" ) {
  $LogFile = "c:\projects\libossia\configure-pd.log"
  cmake $CommonFlags64 -DOSSIA_PD_ONLY=1 c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "pd-32bit" ) {
  $LogFile = "c:\projects\libossia\configure-pd.log"
  cmake $CommonFlags32 -DOSSIA_PD_ONLY=1 c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "pd-test" ) {
  $LogFile = "c:\projects\libossia\configure-pd.log"
  cmake $CommonFlags32 -DOSSIA_PD_ONLY=1 -DOSSIA_TESTING=1 c:\projects\libossia > $LogFile
  CheckLastExitCode

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "python" ) {
  $LogFile = "c:\projects\libossia\configure-${env:APPVEYOR_BUILD_TYPE}-${env:platform}.log"

  if ( "${env:platform}" -eq "x64" ) {

    cd "C:\${env:python}-x64\Scripts"
    pip.exe install wheel
    pip.exe install twine

    cd C:\projects\libossia\build
    cmake $CommonFlags64 -DPYTHON_EXECUTABLE:FILEPATH=C:\${env:python}-x64\python.exe -DPYTHON_LIBRARY=C:\${env:python}-x64\lib${env:python}.a -DOSSIA_PYTHON=1 -DOSSIA_STATIC=1 c:\projects\libossia > $LogFile
    CheckLastExitCode
  } else {

    cd "C:\${env:python}\Scripts"
    pip.exe install wheel
    pip.exe install twine

    cd C:\projects\libossia\build
    cmake $CommonFlags32 -DPYTHON_EXECUTABLE:FILEPATH=C:\${env:python}\python.exe -DPYTHON_LIBRARY=C:\${env:python}\lib${env:python}.a -DOSSIA_STATIC=1 -DOSSIA_PYTHON=1 c:\projects\libossia > $LogFile
    CheckLastExitCode
  }

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "qml" ) {
  if ( Test-Path ${env:QTDIR}\bin\ ) {
    set $env:PATH=${env:QTDIR}\bin;${env:PATH};
  }

  $LogFile = "c:\projects\libossia\configure-${env:APPVEYOR_BUILD_TYPE}.log"
  cmake $CommonFlags64 -DCMAKE_PREFIX_PATH="${env:QTDIR}\lib\cmake\Qt5" -DOSSIA_QML_ONLY=1 c:\projects\libossia > $LogFile
  CheckLastExitCode
}
