#! /usr/bin/env python
# -*- coding: utf-8 -*-


from vplayer.player import VPlayer
from pyossia import ossia_device
from pyossia.functions import ossia_model

# All the VPlayer instances will be created as a model of the ossia_device root
Vplayer = ossia_model(VPlayer, ossia_device)

# create an instance of a video player
vplayer = VPlayer()

# Just a test
from time import sleep
while True:
	pass
	vplayer.play_status = True
	print(vplayer.play_status)
	sleep(1)
	vplayer.play_status = False
	print(vplayer.play_status)
	sleep(1)
