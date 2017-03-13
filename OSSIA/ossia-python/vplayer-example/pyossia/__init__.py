#! /usr/bin/env python
# -*- coding: utf-8 -*-

"""
pyossia module will add usefull access for end users to C++ binded objects of libossia
"""

# Import libossia python bindings
import ossia_python as ossia

# create a list of devices
# access to __devices__ must be done only by using
# pyossia.devices()
__devices__ = {'local':[], 'mirror':[]}

# create a list of datatypes available in OSSIA
# maybe this is not necessary, just because 8'm a bit lazy
datatypes = {	'float':ossia.ValueType.Float,
				'int':ossia.ValueType.Int,
				'bool':ossia.ValueType.Bool,
				'string':ossia.ValueType.String
			}

def devices(device_type='local'):
	# return a list of device
	return __devices__[device_type]

def add_param(self, name, datatype='float'):
	"""
	create a node and make a create_address on the node
	"""
	node = self.add_node(name)
	param = node.create_address(datatypes[datatype])
	return param

def expose(self, protocol='oscquery', udp_port=3456, ws_port=5678):
	"""
	expose the device to the given <protocol>
	"""
	if protocol == 'oscquery':
		self.create_oscquery_server(udp_port, ws_port)
	else:
		print('ossia warning : ' + protocol + ' is not implemented')

def get_nodes(self, node, depth=0):
	"""
	return a list of all nodes attached to the given <node>
	<depth> argument allows a depth-specific list
	depth=0 means explore all the three
	depth=1 means explore only the first level
	(only the children of the given <node>)
	TODO : make depth levels in the code / it does not work for the moment
	"""
	# create an empty list to return
	children = []
	# counter is used to follow depth-leveled exploration
	counter = 0
	# a function to iterate on node's tree recursively
	def iterate_on_children(node):
		"""
		recursive exploration of the tree
		"""
		for child in node.children():
			# add the child to the children list to return
			children.append(child)
			# check the required depth
			if depth == counter:
				break
			# do the same for each child
			counter += 1
			iterate_on_children(child)
	# do the research
	iterate_on_children(node)
	# return the filled list
	return children

def get_params(self, node):
	"""
	return a list of all params for the device
	"""
	children = []
	# a function to iterate on node's tree recursively
	def iterate_on_children(node):
		# check if there is children
		for child in node.children():
			# if the node is a param, it has an address
			if child.get_address():
				# add the child to the children list to return
				children.append(child)
			# do the same for each child
			iterate_on_children(child)
	# do the walk
	iterate_on_children(node)
	# return the filled list
	return children

# customize a bit LocalDevice
# add a new_param /message / return method
# with kwargs as desired (optional)
ossia.LocalDevice.add_param = add_param
ossia.LocalDevice.expose = expose
ossia.LocalDevice.get_nodes = get_nodes
ossia.LocalDevice.get_params = get_params
