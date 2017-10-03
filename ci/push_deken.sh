#!/bin/bash -eux

# the GPG configuration have been setup thanks to : http://www.debonair.io/post/maven-cd/

if [[ "$BUILD_TYPE" == *Pd* ]]; then

  mkdir -p ~/bin
  curl https://raw.githubusercontent.com/pure-data/deken/master/developer/deken > ~/bin/deken
  chmod 755 ~/bin/deken
  # dry run to configure and create ~/.deken
  ~/bin/deken || true

  # decrypt deken configuration file
  openssl aes-256-cbc -K $encrypted_7a0fc0b5101e_key -iv $encrypted_7a0fc0b5101e_iv -in ${0%/*}/deken.config.enc -out ~/.deken/config -d

  # decrypt GPG key
  # openssl aes-256-cbc -K $encrypted_2bca04643b7f_key -iv $encrypted_2bca04643b7f_iv -in ${0%/*}/codesigning.asc.enc -out ${0%/*}/codesigning.asc -d

  # gpg --fast-import ${0%/*}/codesigning.asc

  cd $TRAVIS_BUILD_DIR/ossia-pd-package


  VERSION=v0.0-test3

  mv $TRAVIS_BUILD_DIR/ossia-src-unix.tar.gz "./ossia-v${VERSION}-(Sources)-externals.tar.gz"

  #if [[ "$TRAVIS_TAG" != ""]]; then
    ~/bin/deken upload -v $VERSION --no-source-error ossia
    ~/bin/deken upload "./ossia-v${VERSION}-(Sources)-externals.tar.gz"
  # else
    # ~/bin/deken upload -v $TRAVIS_TAG ossia
  fi
fi

