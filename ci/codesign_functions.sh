codesign_osx() {
  local folder=${1}
  security unlock-keychain -p travis build.keychain
  find $folder -name '*.dylib' -exec codesign --force --timestamp --sign "ossia.io" {} \;
  find $folder -name '*.mxo' -exec codesign --force --timestamp --sign "ossia.io" {} \;
  find $folder -name '*.whl' -exec codesign --force --timestamp --sign "ossia.io" {} \;
  find $folder -name '*.so' -exec codesign --force --timestamp --sign "ossia.io" {} \;
  find $folder -name '*.pd_darwin' -exec codesign --force --timestamp --sign "ossia.io" {} \;
}

notarize_osx() { 
  set +x 
  local zipfile=${1}
  local bundle_id=${2}
  xcrun altool --notarize-app \
      -t osx \
      -f "$zipfile" \
      --primary-bundle-id "$bundle_id" \
      -u jeanmichael.celerier@gmail.com \
      -p "@env:MAC_ALTOOL_PASSWORD"
}

release_macos_folder() {
  local folder="${1}"
  local zipfile="${2}"
  local bundle_id="${3}"

  codesign_osx "$folder"
  (
    cd "$folder"/..
    ditto -c -k --sequesterRsrc --keepParent $(basename "$folder") "${ARTIFACTS_DIR}/$zipfile"
    notarize_osx "${ARTIFACTS_DIR}/$zipfile" "$bundle_id"
  )
}