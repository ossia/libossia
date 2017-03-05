#! /usr/bin/env python
# -*- coding: utf-8 -*-


import ossia_python as ossia


# The function that we will use as a decorator
# It is a generic function of the pyOssia

from functools import wraps

def add_push(function_to_enhance, address=None):
    # this is the ossia address we want to push
    ossia_address = address
    @wraps(function_to_enhance)
    def new_wrapper(self, *args, **kwargs):
        # this is the new value we want to push
        result = args[0]
        # don't forget to call the original function first
        readback = function_to_enhance(self, result)
        # this is the function name
        function = function_to_enhance.__name__
        # make the ossia push
        ossia_address.push_value(ossia.Value(result))
        # return result as the original fset function do
        return readback
    # return the new_wrapper when modify Original Class
    return new_wrapper

def add_pull(function_to_enhance, address=None):
    @wraps(function_to_enhance)
    def new_wrapper(self):
        result = function_to_enhance(self)
        return result
	self.add_callback(bool_value_callback)
    # attach a callback function to the boolean address
    def bool_value_callback(v):
    	result = v.get()
    	return result
	# return the new_wrapper when modify Original Class
    return new_wrapper

def ossia_param(Class, key, OssiaNode):
	"""
	rewrite Original property
	"""
	# create a node for this parameter
	ossia_node = OssiaNode.add_node(key)
	# attach a value to this address
	ossia_address = ossia_node.create_address(ossia.ValueType.Bool)
	# Override the Property of this parameter in the Original Class
	puller = getattr(Class, key)
	puller = getattr(puller, 'fget')
	pusher = getattr(Class, key)
	pusher = getattr(pusher, 'fset')
	setattr(Class, key, property(add_pull(puller, address=ossia_address), add_push(pusher, address=ossia_address)))
	# return the modified parameter
	return getattr(Class, key)

def ossia_return(Class, key, OssiaNode):
	"""
	rewrite Original property
	"""
	# create a node for this parameter
	ossia_node = OssiaNode.add_node(key)
	# attach a value to this address
	ossia_address = ossia_node.create_address(ossia.ValueType.Float)
	ossia_address.set_access_mode(ossia.AccessMode.Get)
	# Override the Property of this parameter in the Original Class
	puller = getattr(Class, key)
	puller = getattr(puller, 'fget')
	setattr(Class, key, property(add_pull(puller, address=ossia_address), None))
	# return the modified parameter
	return getattr(Class, key)

def ossia_message(Class, key, OssiaNode):
	"""
	rewrite Original method
	"""
	# create a node for this parameter
	ossia_node = OssiaNode.add_node(key)
	# attach a value to this address
	ossia_address = ossia_node.create_address(ossia.ValueType.Impulse)
	ossia_address.set_access_mode(ossia.AccessMode.Set)
	# return the modified parameter
	return getattr(Class, key)

def ossia_model(Class, OssiaDevice):
	"""
	rewrite Original Class and make it OSSIA compatible
	"""	
	# create a root node for the VideoPlayer Class (using Class docstring as name)
	# maybe we could only use the first fifteen characters
	#self.model = OssiaDevice.add_node(str(Class.__dict__['__doc__']))
	OssiaNode = OssiaDevice.add_node('toto')
	# make a list of messages (python methods)
	params_and_messages = {key: val for key, val in Class.__dict__.items()if not key.startswith('__')}
	messages = {key:val for key, val in params_and_messages.items() if callable(val)}
	# remove the messages from the global list
	params_and_messages = {key: val for key, val in params_and_messages.items() if not key in messages.keys()}
	# make a list of returns (python properties without fset method)
	returns = {key: val for key, val in params_and_messages.items() if not val.fset}
	# remove the returns from the global list
	params_and_messages = {key: val for key, val in params_and_messages.items() if not key in returns.keys()}
	# make a list of parameters (python properties with fset method)
	params = {key: val for key, val in params_and_messages.items() if isinstance(val, property)}
	print('--- parameters ---')
	for key, val in params.items():
		my_param = ossia_param(Class, key, OssiaNode)
		print(key)
	print('--- messages ---')
	for key, val in messages.items():
		my_message = ossia_message(Class, key, OssiaNode)
		print(key)
		# We need to decorate set method for property to push value to ossia api
	print('--- returns ---')
	for key, val in returns.items():
		my_return = ossia_return(Class, key, OssiaNode)
		print(key)
	return Class

