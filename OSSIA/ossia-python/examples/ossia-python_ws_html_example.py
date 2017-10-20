#! /usr/bin/env python
# -*- coding: utf-8 -*-

import pyossia.ossia_python as ossia
import time

"""
This file illustrates how to control a python with an HTML page though Websocket and thanks to OSSIA.
Run this script and then open ws_client.html file in a browser.
"""

local_device = ossia.LocalDevice("newDevice")
print("local device name: " + local_device.name)

local_device.create_oscquery_server(1234, 5678, False)

# create a node, create a float parameter, set its properties and initialize it
node = local_device.add_node("/color")
parameter = node.create_parameter(ossia.ValueType.Vec3f)
parameter.access_mode = ossia.AccessMode.Bi
parameter.bounding_mode = ossia.BoundingMode.Clip
parameter.make_domain(0,255)
parameter.apply_domain()
parameter.value = [255,0,0]

node = local_device.add_node("/text")
parameter = node.create_parameter(ossia.ValueType.String)
parameter.value = "HELP"

node = local_device.add_node("/speed")
parameter = node.create_parameter(ossia.ValueType.Float)
parameter.access_mode = ossia.AccessMode.Bi
parameter.bounding_mode = ossia.BoundingMode.Clip
parameter.make_domain(-2,2)
parameter.apply_domain()
parameter.value = 1

node = local_device.add_node("/play")
parameter = node.create_parameter(ossia.ValueType.Bool)
parameter.access_mode = ossia.AccessMode.Bi
parameter.value = 0

globq = ossia.GlobalMessageQueue(local_device)

while(True):

  res = globq.pop()
  while res != None:
    parameter, value = res
    print("globq: Got " +  str(parameter.node) + " => " + str(value))
    res=globq.pop()

  time.sleep(0.01)
