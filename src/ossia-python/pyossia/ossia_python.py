#! /usr/bin/env python
# -*- coding: utf-8 -*-

"""
This file is a loader for ossia_python.so file
"""

def __bootstrap__():
    global __bootstrap__, __loader__, __file__
    import pkg_resources, imp
    __file__ = pkg_resources.resource_filename(__name__,'ossia_python.so')
    __loader__ = None; del __bootstrap__, __loader__
    imp.load_dynamic(__name__,__file__)
__bootstrap__()
