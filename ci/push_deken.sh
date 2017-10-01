#!/bin/bash -eux

# the GPG configuration have been setup thanks to : http://www.debonair.io/post/maven-cd/

if [[ "$BUILD_TYPE" == *Pd* ]]; then

  curl https://raw.githubusercontent.com/pure-data/deken/master/developer/deken > ~/bin/deken
  chmod 755 ~/bin/deken
  # dry run to configure and create ~/.deken
  deken || true

  # decrypt deken configuration file
  openssl aes-256-cbc -K $encrypted_7a0fc0b5101e_key -iv $encrypted_7a0fc0b5101e_iv -in ${0%/*}/deken.config.enc -out ~/.deken/config -d

  # decrypt GPG key
  openssl aes-256-cbc -K $encrypted_7a0fc0b5101e_key -iv $encrypted_7a0fc0b5101e_iv -in {0%/*}/signingkey.asc.enc -out {0%/*}/signingkey.asc -d
  gpg --fast-import {0%/*}/signingkey.asc

  cd $TRAVIS_BUILD_DIR/ossia-pd-package
  deken upload -v $TRAVIS_TAG ossia

fi

