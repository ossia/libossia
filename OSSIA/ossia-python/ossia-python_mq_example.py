#! /usr/bin/env python
# -*- coding: utf-8 -*-

import ossia_python as ossia
import time

local_device = ossia.LocalDevice("newDevice")
print("local device name: " + local_device.name)

local_device.create_oscquery_server(3456, 5678, True)

# create a node, create a float parameter, set its properties and initialize it
node = local_device.add_node("/test/numeric/float")
parameter = node.create_parameter(ossia.ValueType.Float)
parameter.access_mode = ossia.AccessMode.Bi
parameter.bounding_mode = ossia.BoundingMode.Clip
parameter.value = 2.5

def value_callback0(val):
  print(str(val))

parameter.add_callback(value_callback0)
node = local_device.add_node("/test/str")
parameter = node.create_parameter(ossia.ValueType.String)
parameter.value = "a string"

def value_callback(node, val):
  print(str(node) + ": " + str(val))
parameter.add_callback_param(value_callback)

messq = ossia.MessageQueue(local_device)
messq.register(parameter)

while(True):
  res = messq.pop()
  if(res != None):
    print("Got " +  str(res[0].node) + " => " + str(res[1]))
    time.sleep(0.1)
