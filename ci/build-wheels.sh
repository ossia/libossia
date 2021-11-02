#!/bin/bash
set -e -x

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" wheel /io/build/src/ossia-python/ -w ${TRAVIS_BUILD_DIR}/build/src/ossia-python/dist
done

for PYBIN in /opt/python/*/bin/; do
    "${PYBIN}/python" get-pip.py
    "${PYBIN}/pip" install setuptools
	$CMAKE_BIN -DCMAKE_BUILD_TYPE=Release -DOSSIA_PYTHON_ONLY=1 ..

	$CMAKE_BIN --build . -- -j2
	# now we just want to install the wheel and run the tests
	${PYBIN} -m pip install --user ${TRAVIS_BUILD_DIR}/build/src/ossia-python/dist/pyossia*.whl
	${PYBIN} ${TRAVIS_BUILD_DIR}/src/ossia-python/tests/test.py


if [[ "${TRAVIS_TAG}" != "x" ]]; then
    for WHEEL in ${TRAVIS_BUILD_DIR}/build/src/ossia-python/dist; do
        ${PYBIN} -m twine upload -u ${PYPIUSER} -p ${PYPIWORD} WHEEL
    done
fi
