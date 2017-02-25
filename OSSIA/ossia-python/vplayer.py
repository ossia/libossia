import ossia_python as ossia

class VPlayer(object):
	"""
	Video Player
	"""
	def __init__(self, device):
		super(VPlayer, self).__init__()
		# init play property to False
		self._play_status = device.add_node('play_status')
		self._play_status = self._play_status.create_address(ossia.ValueType.Bool)
		self._play_status.push_value(ossia.Value(False))
		self._play = device.add_node('play')
		self._play = self._play.create_address(ossia.ValueType.Impulse)
		self._play.set_access_mode(ossia.AccessMode.Set)
		self._stop = device.add_node('stop')
		self._stop = self._stop.create_address(ossia.ValueType.Impulse)
		self._stop.set_access_mode(ossia.AccessMode.Set)
		self._elapsed = device.add_node('elapsed')
		self._elapsed = self._elapsed.create_address(ossia.ValueType.Float)
		self._elapsed.set_access_mode(ossia.AccessMode.Get)
	

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
		return self.elapsed.fetch_value()

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


# create the Video Player Device
my_device = ossia.LocalDevice('my_video_device')
my_device.create_oscquery_server(22222, 33333)
# create an instance of a video player
vplayer = VPlayer(my_device)



from time import sleep

while True:
	vplayer.play_status = True
	sleep(1)
	vplayer.play_status = False
	sleep(1)
