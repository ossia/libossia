import ossia_python as ossia
from vplayer.player import VPlayer

# create the Video Player Device
my_device = ossia.LocalDevice('my_video_device')
my_device.create_oscquery_server(22222, 5678)

from ossia_decorator import ossia_model
# All the VPlayer instances will be created as a model of the OssiaDevice root
Vplayer = ossia_model(VPlayer, my_device)

# create an instance of a video player
vlayer = VPlayer()
#print(dir(vlayer))

from time import sleep

while True:
	vlayer.play_status = True
	sleep(1)
	vlayer.play_status = False
	sleep(1)
