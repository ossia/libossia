#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
setup for the pyossia project
"""

# Always prefer setuptools over distutils
from setuptools import setup, find_packages

# To use a consistent encoding
from codecs import open
from os import path
HERE = path.abspath(path.dirname(__file__))
# Get the long description from the README file
with open(path.join(HERE, 'README.rst'), encoding='utf-8') as f:
    LONG_DESCRIPTION = f.read()

# get current version
import versioneer
__version__ = versioneer.get_version()

from setuptools.extension import Extension

ossia_python = Extension(
    name='ossia_python',
    include_dirs=['./../'],
    sources=['./ossia_python.cpp'],
    extra_compile_args=['-std=c++14'],
)

setup(
    name = 'pyossia',
    version =__version__,
    description = 'libossia is a modern C++, cross-environment distributed object model for creative coding and interaction scoring Edit',
    long_description = LONG_DESCRIPTION,
    url = 'https://github.com/OSSIA/libossia/ossia-python',
    author = 'OSSIA team',
    author_email = 'contact@ossia.io',
    license ='GPLv3+',
    classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Topic :: Software Development :: Libraries :: Python Modules'
    ],
    keywords = ['creative', 'controls', 'osc', 'oscquery', 'websocket', 'libossia', 'midi'],
    packages = find_packages(),
    ext_modules=[ossia_python],
    include_package_data=True,
    zip_safe=False
)
