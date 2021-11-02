#!/bin/bash
set -ex

REPO_ROOT="${0%/*}/../"

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" wheel /io/build/src/ossia-python/ -w ${REPO_ROOT}/build/src/ossia-python/dist
done

for PYBIN in /opt/python/*/bin/; do
    "${PYBIN}/python" get-pip.py
    "${PYBIN}/pip" install setuptools
	$CMAKE_BIN -DCMAKE_BUILD_TYPE=Release -DOSSIA_PYTHON_ONLY=1 ..

	$CMAKE_BIN --build . -- -j2
	# now we just want to install the wheel and run the tests
	${PYBIN} -m pip install --user ${REPO_ROOT}/build/src/ossia-python/dist/pyossia*.whl
	${PYBIN} ${REPO_ROOT}/src/ossia-python/tests/test.py
if [[ ${GITHUB_REF} == refs/tags/* ]]; then
    for WHEEL in ${REPO_ROOT}/build/src/ossia-python/dist; do
        ${PYBIN} -m twine upload -u ${PYPIUSER} -p ${PYPIWORD} WHEEL
    done
fi
