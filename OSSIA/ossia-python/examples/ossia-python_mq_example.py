#! /usr/bin/env python
# -*- coding: utf-8 -*-

import ossia_python as ossia
import time

local_device = ossia.LocalDevice("newDevice")
print("local device name: " + local_device.name)

local_device.create_oscquery_server(1234, 5678, False)

messq = ossia.MessageQueue(local_device)
globq = ossia.GlobalMessageQueue(local_device)

# create a node, create a float parameter, set its properties and initialize it
node = local_device.add_node("/test/numeric/float")
parameter = node.create_parameter(ossia.ValueType.Float)
parameter.access_mode = ossia.AccessMode.Bi
parameter.bounding_mode = ossia.BoundingMode.Clip
parameter.value = 2.5

messq.register(parameter)

node = local_device.add_node("/test/str")
parameter = node.create_parameter(ossia.ValueType.String)
parameter.value = "a string"

while(True):
  res = messq.pop()
  if(res != None):
    parameter, value = res
    print("messq: Got " +  str(parameter.node) + " => " + str(value))

  res = globq.pop()
  if(res != None):
    parameter, value = res
    print("globq: Got " +  str(parameter.node) + " => " + str(value))

  time.sleep(0.1)
