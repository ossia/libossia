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
  cd ${env:APPVEYOR_BUILD_FOLDER}\build

  if ( $env:configuration -eq "Release" ){
    mkdir ${env:APPVEYOR_BUILD_FOLDER}\build\Test\Release
    copy ${env:APPVEYOR_BUILD_FOLDER}\build\OSSIA\Release\ossia.dll ${env:APPVEYOR_BUILD_FOLDER}\build\Tests\Release\
  } else {
    mkdir ${env:APPVEYOR_BUILD_FOLDER}\build\Test\Debug
    copy ${env:APPVEYOR_BUILD_FOLDER}\build\OSSIA\Debug\ossia.dll ${env:APPVEYOR_BUILD_FOLDER}\build\Tests\Debug\
  }
} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "Release" ){

  cd ${env:APPVEYOR_BUILD_FOLDER}\build
  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-${env:APPVEYOR_BUILD_TYPE}-win64.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode
  cd ${env:APPVEYOR_BUILD_FOLDER}\install
  ls

  7z a ${env:APPVEYOR_BUILD_FOLDER}\libossia-native-win64.zip .

  cd ${env:APPVEYOR_BUILD_FOLDER}\build-32bit
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
  cd ${env:APPVEYOR_BUILD_FOLDER}\build

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-pd.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd ${env:APPVEYOR_BUILD_FOLDER}\install\ossia-pd-package\
  ls .

  7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-pd-win32.zip ossia



  if (${env:APPVEYOR_REPO_TAG}){
    $env:PATH="C:\msys64\usr\bin;${env:PATH}"

    $VERSION=${env:APPVEYOR_REPO_TAG_NAME}

    nuget install secure-file -ExcludeVersion
    secure-file\tools\secure-file -decrypt ${env:APPVEYOR_BUILD_FOLDER}\ci\codesigning.asc.appveyor.enc -out ${env:APPVEYOR_BUILD_FOLDER}\ci\codesigning.asc -secret ${env:GPG_DECODE_KEY}

    gpg.exe --fast-import ${env:APPVEYOR_BUILD_FOLDER}\ci\codesigning.asc

    curl.exe --user "ossia:${env:DEKEN_PASSWORD}" -X MKCOL  "https://puredata.info/Members/ossia/software/ossia/${VERSION}/"
    $ARCHIVE_NAME="ossia-v${VERSION}-(W32-i386-32)-externals.zip"
    copy ${env:APPVEYOR_BUILD_FOLDER}\ossia-pd-win32.zip ${ARCHIVE_NAME}

    gpg.exe -ab --batch --yes ${ARCHIVE_NAME}
    $SHA = sha256.exe ${ARCHIVE_NAME}
    $SHA.substring(0,64) > "${ARCHIVE_NAME}.sha"

    curl.exe --user ossia:${env:DEKEN_PASSWORD} -T "${ARCHIVE_NAME}"     "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}"     --basic
    curl.exe --user ossia:${env:DEKEN_PASSWORD} -T "${ARCHIVE_NAME}.asc" "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}.asc" --basic
    curl.exe --user ossia:${env:DEKEN_PASSWORD} -T "${ARCHIVE_NAME}.sha" "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}.sha" --basic
  }
} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "qml" ){
  cd ${env:APPVEYOR_BUILD_FOLDER}\build

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-qml.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd ${env:APPVEYOR_BUILD_FOLDER}\install\
  ls

  7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-qml-win64.zip .

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "python" ){

  if (${env:APPVEYOR_REPO_TAG}){
    ${PYTHON_EXE}="C:\${env:python}\python.exe"
    if ( "${env:platform}" -eq "x64" ){
      ${PYTHON_EXE}="C:\${env:python}-x64\python.exe"
    }
    ${PYTHON_EXE} -m twine upload ${env:APPVEYOR_BUILD_FOLDER}/build/OSSIA/ossia-python/dist/pyossia*.whl
  }

  mv ${env:APPVEYOR_BUILD_FOLDER}/build/OSSIA/ossia-python/dist/pyossia*.whl ${env:APPVEYOR_BUILD_FOLDER}/

} elseif ( $env:APPVEYOR_BUILD_TYPE -eq "max" ){

  cd ${env:APPVEYOR_BUILD_FOLDER}\build

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-max.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd ${env:APPVEYOR_BUILD_FOLDER}\build-32bit

  $LogFile = "${env:APPVEYOR_BUILD_FOLDER}\install-max-32bit.log"
  cmake --build . --config "${env:configuration}" --target install > "$LogFile"
  CheckLastExitCode

  cd ${env:APPVEYOR_BUILD_FOLDER}\install\ossia-max-package\
  ls

  7z a ${env:APPVEYOR_BUILD_FOLDER}\ossia-max-win.zip .
}
