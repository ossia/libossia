#! /usr/bin/env python
# -*- coding: utf-8 -*-


import ossia_python as ossia

# The class that we want to expose to libossia
# we want to add it as a OSSIA MODEL, without editing this class
class VPlayer(object):
	"""
	Video Player
	"""
	def __init__(self):
		super(VPlayer, self).__init__()
		# init play property to False
		self._play_status = False

	def play(self):
		"""
		Play the video
		"""
		self.play_status = True

	def pause(self):
		"""
		Pause the video
		"""
		self.play_status = inverse(self.play_status)

	def stop(self):
		"""
		Play the video
		"""
		self.play_status = False

	@property
	def elapsed(self):
		"""
		Time ......
		"""
		return self._elapsed

	@property
	def play_status(self):
		"""
		play_status
		True make it play
		False make it stop
		"""
		return self._play_status
	@play_status.setter
	def play_status(self, play_status):
		self._play_status = play_status




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
        function = function_to_enhance.__name__
        result = function_to_enhance(self)
		# make a connection for the ossia pull
        #print(str(self) + ' make a pull for -' + function)
        # return result as the original fget function do
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
	"""
	print('--- messages ---')
	for key, val in messages.items():
		print('-  ' + key + ' : ' + str(val))
		setattr(Class, key, property(add_push(getattr(Class, key))))
		# add messages to the model
		my_message = self.model.add_node(str(key))
		my_message = my_message.create_address(ossia.ValueType.Impulse)
		my_message.set_access_mode(ossia.AccessMode.Set)
		# We need to decorate set method for property to push value to ossia api
	print('--- returns ---')
	for key, val in returns.items():
		print('-  ' + key + ' : ' + str(val))
		puller = getattr(Class, key)
		puller = getattr(puller, 'fget')
		setattr(Class, key, property(add_pull(puller)))
		# add returns to the model
		my_return = self.model.add_node(str(key))
		my_return = my_return.create_address(ossia.ValueType.Float)
		# We need to decorate get method for property to pull value to ossia api
		my_return.set_access_mode(ossia.AccessMode.Get)
	"""
	return Class

# This is the only code we need to declare an OSSIA DEVICE
# create the Video Player Device
OssiaDevice = ossia.LocalDevice('my_video_device')
OssiaDevice.create_oscquery_server(22222, 33333)
# All the VPlayer instances will be created as a model of the OssiaDevice root
Vplayer = ossia_model(VPlayer, OssiaDevice)

# create an instance of a video player
vplayer = VPlayer()

# Just a test
from time import sleep
while True:
	vplayer.play_status = True
	print(vplayer.play_status)
	sleep(1)
	vplayer.play_status = False
	print(vplayer.play_status)
	sleep(1)
