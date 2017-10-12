#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
setup for the pyossia project
"""

# Always prefer setuptools over distutils
from setuptools import setup, find_packages, Command
# To use a consistent encoding
from codecs import open
from os import path
import subprocess
from distutils.command.build import build as _build
HERE = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(HERE, 'README.rst'), encoding='utf-8') as f:
    LONG_DESCRIPTION = f.read()

# get current version
import versioneer
__version__ = versioneer.get_version()


try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel
    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            _bdist_wheel.finalize_options(self)
            self.root_is_pure = False
except ImportError:
    bdist_wheel = None

setup(
    name = 'pyossia',
    version =__version__,
    description = 'libossia usefull Classes',
    long_description = LONG_DESCRIPTION,
    url = 'https://github.com/PixelStereo/pyossia',
    author = 'Pixel Stereo',
    author_email = 'contact@pixelstereo.org',
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
    keywords = ['creative', 'controls', 'osc', 'oscquery', 'websocket', 'libossia'],
    packages = find_packages(),
    cmdclass=versioneer.get_cmdclass(),
    package_data={
        'pyossia': ['ossia_python.so'],
    },
    zip_safe=False
)
