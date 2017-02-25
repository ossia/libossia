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

def ossia_model(Class, OssiaDevice):
	print('--- EXPOSE DEVICE or NODE ---')
	class OssiaModel(object):
		"""
		This is an OssiaModel
		"""
		def __init__(self, Class, OssiaDevice):
			super(OssiaModel, self).__init__()
			# create a root node for the VideoPlayer Class (using Class docstring as name)
			# maybe we could only use the first fifteen characters
			self.model = OssiaDevice.add_node(str(Class.__dict__['__doc__']))
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
			self.params = []
			for key, val in params.items():
				print('-  ' + key + ' : ' + str(val))
				my_param = OssiaDevice.add_node(str(key))
				my_param = my_param.create_address(ossia.ValueType.Bool)
				my_param.push_value(ossia.Value(False))
				print(my_param)
			print('--- messages ---')
			self.messages = []
			for key, val in messages.items():
					print('-  ' + key + ' : ' + str(val))
					# add messages to the model
					my_message = self.model.add_node(str(key))
					my_message.create_address(ossia.ValueType.Impulse)
					my_message.set_access_mode(ossia.AccessMode.Set)
					self.messages.append(my_message)
			print('--- returns ---')
			self.returns = []
			for key, val in returns.items():
					print('-  ' + key + ' : ' + str(val))
					# add returns to the model
					my_return = self.model.add_node(str(key))
					my_return.create_address(ossia.ValueType.Float)
					my_return.set_access_mode(ossia.AccessMode.Get)
					self.returns.append(my_return)
	return OssiaModel(Class, OssiaDevice)


# The code that we will use

# create the Video Player Device
OssiaDevice = ossia.LocalDevice('my_video_device')
OssiaDevice.create_oscquery_server(22222, 33333)

# Expose the VPlayer Class as a OSSIA MODEL
Vplayer = ossia_model(VPlayer, OssiaDevice)
# create an instance of a video player
vplayer = VPlayer()


# Just a test
from time import sleep

while True:
	vplayer.play_status = True
	print(True)
	sleep(1)
	vplayer.play_status = False
	print(False)
	sleep(1)
