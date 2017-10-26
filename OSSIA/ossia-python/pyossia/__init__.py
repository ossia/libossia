#! /usr/bin/env python
# -*- coding: utf-8 -*-

"""
Introduction
============
pyossia module will add usefull access for end users to C++ binded objects of libossia


Change log
==========
0.0.* aka the first
*******************
First version of pyossia, still in alpha develeopment.


pyossia methods
===============
"""


# Import libossia python bindings
# the ossia_python.so file must be in the pyossia module
from . import ossia_python as ossia

# these few lines are used to get versionning from git
from ._version import get_versions
__version__ = get_versions()['version']
del get_versions

print('pyossia ' + __version__)

######################################################
# Module Constants
######################################################

# create a list of value_types available in OSSIA
# maybe this is not necessary, just because 8'm a bit lazy
__value_types__ = {'float':ossia.ValueType.Float,
                   'int':ossia.ValueType.Int,
                   'bool':ossia.ValueType.Bool,
                   'string':ossia.ValueType.String,
                   'impulse':ossia.ValueType.Impulse,
                   'list':ossia.ValueType.List,
                   'vec2f':ossia.ValueType.Vec2f,
                   'vec3f':ossia.ValueType.Vec3f,
                   'vec4f':ossia.ValueType.Vec4f,
                   'char':ossia.ValueType.Char,
                  }

# create a list of devices
# access to __devices__ must be done only by using
# add_device and pyossia.devices() (todo : add remove_device)
__devices__ = {'local':[], 'mirror':[]}

######################################################
# Module functions / shortcuts to access libossia
######################################################

def add_device(name, **kwargs):
    """
    create a local device
    The local device might be a local device or a mirror (remote) device
    depending on the "mode" argument
    mode = local / mirror
    """
    # TODO :  raise an exception if mode is not provided as kwargs
    mode = kwargs['mode']
    if mode == 'local':
        device = ossia.LocalDevice(name)
    elif mode == 'mirror':
        target = kwargs['target']
        udp_port = kwargs['udp_port']
        device = ossia.OSCQueryDevice(name, target, udp_port)
    else:
        print(mode + ' is not implemented')
    __devices__[mode].append(device)
    return device

def devices(device_type='local'):
    """
    return a list of devices
    """
    return __devices__[device_type]


def expose(self, protocol='oscquery', host='localhost', listening_port=3456, sending_port=5678, logger=False):
    """
    expose the device to the given <protocol>

    # TODO : Implement other protocol (serial, midi, osc, etc…)
    """
    if protocol == 'oscquery':
        self.create_oscquery_server(listening_port, sending_port, logger)
    elif protocol == 'osc':
        self.create_osc_server(host, listening_port, sending_port, logger)
    else:
        print('ossia warning : ' + protocol + ' is not implemented')

######################################################
# Following functions are here for conveniance over ossia_python bindings
# and might be add to libossia bindings later.
######################################################

def add_param(self, name, **kwargs):
    """
    create a node and make a create_parameter on the node
    """
    node = self.add_node(name)
    value_type = kwargs['value_type']
    param = node.create_parameter(__value_types__[value_type])
    if 'domain' in kwargs.keys():
        # this one needs to receive a tuple, not a list
        # TODO : this might be changed in ossia_python.cpp to accept a list??
        param.make_domain(tuple(kwargs['domain']))
        param.apply_domain()
    if 'default_value' in kwargs.keys():
        param.default_value = kwargs['default_value']
    if 'description' in kwargs.keys():
        param.node.description = kwargs['description']
    if 'bounding_mode' in kwargs.keys():
        param.bounding_mode = kwargs['bounding_mode']
    if 'unit' in kwargs.keys():
        param.unit = kwargs['unit]']
    if 'repetition_filter_label' in kwargs.keys():
        param.repetition_filter_label = kwargs['repetition_filter_label']
    return param

def get_nodes(self, node=None, depth=0):
    """
    return a list of all nodes attached to the given <node>.

    <depth> argument allows a depth-specific list

    depth=0 means explore all the three

    depth=1 means explore only the first level

    (only the children of the given <node>)

    TODO : make depth levels in the code / it does not work for the moment
    # check the required depth
    #counter += 1
    #if depth == counter and depth != 0:
    #    break
    """
    if not node:
        node = self
    # create an empty list to return
    children = []
    # counter is used to follow depth-leveled exploration
    counter = 0
    # a function to iterate on node's tree recursively
    def iterate_nodes(node, counter):
        """
        recursive exploration of the tree
        """
        for child in node.children():
            # if the node is a param, it has an parameter
            if not child.parameter.__class__.__name__ == 'Parameter':
                # add the child to the children list to return
                children.append(child)
            # do the same for each child
            iterate_nodes(child, counter)
    # do the research
    iterate_nodes(node, counter)
    # return the filled list
    return children

def get_parameters(self, node=None, depth=0):
    """
    return a list of all params for the device
    depth = 0 returns parameters recursivly for all its children
    depth = 1 returns only the parameters for this node
    depth = 2 returns only the parameters for this node and its children
    """
    if not node:
        node = self
    parameters = []
    # a function to iterate on node's tree recursively
    def iterate_parameters(node):
        """
        recursive function to explore the whole three
        """
        # check if there is children
        for child in node.children():
            # if the node is a param, it has an parameter
            if child.parameter.__class__.__name__ == 'Parameter':
                # add the child to the children list to return
                parameters.append(child.parameter)
            if not depth:
                # do the same for each child
                iterate_parameters(child)
    # do the walk
    iterate_parameters(node)
    # return the filled list
    return parameters

def init(self):
    """
    resets all parameters of this node to the default_value
    """
    for param in self.get_parameters():
        if param.default_value:
            param.value = param.default_value

def reset(self):
    """
    reset a parameter to its default value
    """
    if self.default_value:
        self.value = self.default_value


# customize a bit LocalDevice
# add a new_param /message / return method
# with kwargs as desired (optional)
ossia.LocalDevice.add_param = add_param
ossia.LocalDevice.expose = expose

# OSCQueryDevice is a mirror
# your cannot create nodes and parameters
ossia.OSCQueryDevice.get_nodes = get_nodes
ossia.OSCQueryDevice.get_parameters = get_parameters

# A Node has nodes and parameters
ossia.Node.get_nodes = get_nodes
ossia.Node.get_parameters = get_parameters
ossia.Node.init = init

# A Parameter can be reset to its default_value
ossia.Parameter.reset = reset
#ossia.Parameter.description = ossia.Node.description