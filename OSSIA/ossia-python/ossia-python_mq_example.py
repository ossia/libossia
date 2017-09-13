#! /usr/bin/env python
# -*- coding: utf-8 -*-

import ossia_python as ossia
import time

local_device = ossia.LocalDevice("newDevice")
print("local device name: " + local_device.name)

local_device.create_oscquery_server(3456, 5678, True)

# create a node, create a float parameter, set its properties and initialize it
float_node = local_device.add_node("/test/numeric/float")
float_parameter = float_node.create_parameter(ossia.ValueType.Float)
float_parameter.access_mode = ossia.AccessMode.Bi
float_parameter.bounding_mode = ossia.BoundingMode.Clip
float_parameter.value = ossia.Value(2.5)                  ### TODO : float_parameter.value = 2.5


messq = ossia.MessageQueue(local_device)
messq.register(float_parameter)

while(True):
  res = messq.pop()
  if(res != None):
    print("Got " +  str(res[0].node) + " => " + str(res[1]))
    time.sleep(0.1)
