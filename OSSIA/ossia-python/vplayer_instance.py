import ossia_python as ossia

class VPlayer(object):
	"""
	Video Player
	"""
	def __init__(self):
		super(VPlayer, self).__init__()
		# create the Video Player Device
		self._me = ossia.LocalDevice('VPlayer')
		self._me.create_oscquery_server(22222, 33333)
		# init play property to False
		self._play_status = self._me.add_node('play_status')
		self._play_status = self._play_status.create_address(ossia.ValueType.Bool)
		self._play_status.push_value(ossia.Value(False))


	@property
	def play_status(self):
		"""
		play_status
		True make it play
		False make it stop
		"""
		return self._play_status.fetch_value()
	@play_status.setter
	def play_status(self, play_status):
		self._play_status.push_value(ossia.Value(play_status))


params = {key: val for key, val in VPlayer.__dict__.items()}
#params = {key: val for key, val in VPlayer.__dict__.items()if not key.startswith('__')}
print(params)
print(VPlayer.__dict__['__doc__'])
print('-------------')
print()
# this is a parameter so it can be get or set
print(params['play_status'].setter)
print(params['play_status'].getter)

#for key, val in VPlayer.__dict__.items():
#	print(key, val)
