#!/bin/bash
set -e -x

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" wheel /io/build/OSSIA/ossia-python/ -w ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist
done

for PYBIN in /opt/python/*/bin/; do
    "${PYBIN}/python" get-pip.py
    "${PYBIN}/pip" install setuptools
	$CMAKE_BIN -DCMAKE_C_COMPILER="$CC" \
      -DCMAKE_CXX_COMPILER="$CXX" \
      -DBOOST_ROOT="$BOOST_ROOT" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX="$TRAVIS_BUILD_DIR/ossia-python" \
      -DPYTHON_EXECUTABLE=${PYBIN}/python \
      -DOSSIA_STATIC=1 \
      -DOSSIA_TESTING=0 \
      -DOSSIA_EXAMPLES=0 \
      -DOSSIA_CI=1 \
      -DOSSIA_PD=0 \
      -DOSSIA_QT=0 \
      -DOSSIA_QML=0 \
      -DOSSIA_PYTHON=1 ..

	$CMAKE_BIN --build . -- -j2	
	# now we just want to install the wheel and run the tests
	${PYBIN} -m pip install --user ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist/pyossia*.whl
	${PYBIN} ${TRAVIS_BUILD_DIR}/OSSIA/ossia-python/tests/test.py


if [[ "${TRAVIS_TAG}" != "x" ]]; then
    for WHEEL in ${TRAVIS_BUILD_DIR}/build/OSSIA/ossia-python/dist; do
        ${PYBIN} -m twine upload -u ${PyPiUser} -p ${PyPiWord} WHEEL
    done
fi