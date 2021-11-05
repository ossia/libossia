#!/bin/bash
set -ex

REPO_ROOT="${0%/*}/../"
PYBIN=python3

echo "Building for python: $PYBIN"

# Compile wheels
${PYBIN} -m pip wheel /io/build/src/ossia-python/ -w ${REPO_ROOT}/build/src/ossia-python/dist

if [[ ${GITHUB_REF} == refs/tags/* ]]; then
    for WHEEL in ${REPO_ROOT}/build/src/ossia-python/dist; do
        ${PYBIN} -m twine upload -u ${PYPIUSER} -p ${PYPIWORD} WHEEL
    done
fi
