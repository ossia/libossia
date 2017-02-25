import ossia_python as ossia

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


def ossia_model(Class, my_device):
	print('--- EXPOSE DEVICE or NODE ---')
	print(Class.__dict__['__doc__'])
	params_and_messages = {key: val for key, val in Class.__dict__.items()if not key.startswith('__')}
	messages = {key:val for key, val in params_and_messages.items() if callable(val)}
	params_and_messages = {key: val for key, val in params_and_messages.items() if not key in messages.keys()}
	returns = {key: val for key, val in params_and_messages.items() if not val.fset}
	params_and_messages = {key: val for key, val in params_and_messages.items() if not key in returns.keys()}
	params = {key: val for key, val in params_and_messages.items() if isinstance(val, property)}
	print('--- parameters ---')
	for key, val in params.items():
		print('-  ' + key + ' : ' + str(val))
		my_param = my_device.add_node(str(key))
		my_param = my_param.create_address(ossia.ValueType.Bool)
		my_param.push_value(ossia.Value(False))
		print(my_param)
	print('--- messages ---')
	for key, val in messages.items():
			print('-  ' + key + ' : ' + str(val))
			#my_device.add_node(str(key)).create_address(ossia.ValueType.Message).push_value(ossia.Value(False))
	print('--- returns ---')
	for key, val in returns.items():
			print('-  ' + key + ' : ' + str(val))
			#my_device.add_node(str(key)).create_address(ossia.ValueType.Return).push_value(ossia.Value(False))
	return Class


# create the Video Player Device
my_device = ossia.LocalDevice('my_video_device')
my_device.create_oscquery_server(22222, 33333)

# Expose the VPlayer Class as a OSSIA MODEL
Vplayer = ossia_model(VPlayer, my_device)
# create an instance of a video player
vplayer = VPlayer()

from time import sleep

while True:
	vplayer.play_status = True
	print(True)
	sleep(1)
	vplayer.play_status = False
	print(False)
	sleep(1)






quit()


from functools import wraps

class Model(object):
	"""
	This is an ossia model
	A model is a hierarchik bunch of parameters / messages / returns
	This Class instanciate them to the Ossia Node Lib
	"""
	def __init__(self, view_func, device):
		self.view_func = view_func
        wraps(view_func)(self)
	
	def __call__(self, request, *args, **kwargs):
		# maybe do something before the view_func call
		response = self.view_func(request, *args, **kwargs)
		# maybe do something after the view_func call
		return response
