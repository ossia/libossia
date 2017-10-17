#!/bin/bash -ux

# the GPG configuration have been setup thanks to : http://www.debonair.io/post/maven-cd/

#if [[ "$BUILD_TYPE" == *Pd* && "$TRAVIS_TAG" != "" ]]; then
if [[ "$BUILD_TYPE" == *Pd* ]]; then

  # decrypt GPG key
  openssl aes-256-cbc -K $encrypted_7a0fc0b5101e_key -iv $encrypted_7a0fc0b5101e_iv -in ${0%/*}/codesigning.asc.enc -out ${0%/*}/codesigning.asc -d

  gpg --fast-import ${0%/*}/codesigning.asc

  cd $TRAVIS_BUILD_DIR/ossia-pd-package

  VERSION="test-${TRAVIS_TAG}"
  if [[ "$TRAVIS_TAG" != "" ]]; then
    VERSION=${TRAVIS_TAG}
  fi

  #Create folder on Webdav
  curl --user '${DEKEN_USER}:${DEKEN_PASSWORD}'  -X MKCOL  "https://puredata.info/Members/ossia/software/ossia/$VERSION/"

  ARCHIVE_NAME="./ossia-v${VERSION}-(Sources)-externals.tar.gz"
  mv $TRAVIS_BUILD_DIR/ossia-src-unix.tar.gz ${ARCHIVE_NAME}
  read HASH FILE <<< `sha256sum "${ARCHIVE_NAME}"`
  echo $HASH > $FILE.sha256
  gpg -ab ${ARCHIVE_NAME}

  if [[ "$BUILD_TYPE" == "PdRelease" ]]; then
    curl --user '${DEKEN_USER}:${DEKEN_PASSWORD}'  -T "${ARCHIVE_NAME}"        "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}" --basic
    curl --user '${DEKEN_USER}:${DEKEN_PASSWORD}'  -T "${ARCHIVE_NAME}.sha256" "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}.sha256" --basic
    curl --user '${DEKEN_USER}:${DEKEN_PASSWORD}'  -T "${ARCHIVE_NAME}.asc"    "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}.asc" --basic
  fi

  OS="(Linux-amd64-64)"
  if [[ "$BUILD_TYPE" == *Rpi* ]]; then
    OS="(Linux-arm-32)"
  fi

  if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    OS="(Darwin-x86_64-64)"
  fi

  ARCHIVE_NAME="ossia-v${VERSION}-(${OS})-externals.tar.gz"
  tar -czf ${ARCHIVE_NAME}
  read HASH FILE <<< `sha256sum "${ARCHIVE_NAME}"`
  echo $HASH > $FILE.sha256

  curl --user '${DEKEN_USER}:${DEKEN_PASSWORD}'  -T "${ARCHIVE_NAME}"        "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}"        --basic
  curl --user '${DEKEN_USER}:${DEKEN_PASSWORD}'  -T "${ARCHIVE_NAME}.sha256" "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}.sha256" --basic
  curl --user '${DEKEN_USER}:${DEKEN_PASSWORD}'  -T "${ARCHIVE_NAME}.asc"    "https://puredata.info/Members/ossia/software/ossia/${VERSION}/${ARCHIVE_NAME}.asc" --basic

fi
