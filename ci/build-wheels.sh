#!/bin/bash
set -e -x

# Install a system package required by our library
yum install -y atlas-devel

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" install -r /io/dev-requirements.txt
    "${PYBIN}/pip" wheel /io/build/OSSIA/ossia-python/ -w wheelhouse/
done

# Bundle external shared libraries into the wheels
for whl in wheelhouse/*.whl; do
    auditwheel repair "$whl" -w /io/wheelhouse/
done

# Install packages and test
for PYBIN in /opt/python/*/bin/; do
    "${PYBIN}/pip" install pyossia --no-index -f /io/wheelhouse
    (cd "$HOME"; "${PYBIN}" /io/)
done

if [[ "x${TRAVIS_TAG}" != "x" ]]; then
	for WHEEL in /io/wheelhouse; do
  		python -m twine upload -u ${PyPiUser} -p ${PyPiWord} WHEEL
  	done
fi