#! /usr/bin/env python
# -*- coding: utf-8 -*-

"""
This file illustrates how to use the python binding of Ossia library.
This is a low level approach to integrate Ossia architecture into a python program.
For higher level features use pyossia module.
"""

import ossia_python as ossia
import time

print("OSSIA LIBRARY EXAMPLE")

### LOCAL DEVICE SETUP

# create a device for this python program
local_device = ossia.LocalDevice("newDevice")

# enable OSCQuery communication for our device
local_device.create_oscquery_server(3456, 5678)



# create a node, create a boolean parameter and initialize it
bool_node = local_device.add_node("/test/value/bool")
bool_parameter = bool_node.create_parameter(ossia.ValueType.Bool)
### TODO : bool_parameter = bool_node.create_parameter(type='Bool', default=True, repetitions=False)
bool_parameter.access_mode = ossia.AccessMode.Get
bool_parameter.value = ossia.Value(True)                  ### TODO : bool_parameter.value = True


# create a node, create an integer parameter and initialize it
int_node = local_device.add_node("/test/value/int")
int_parameter = int_node.create_parameter(ossia.ValueType.Int)

int_parameter.access_mode = ossia.AccessMode.Set
int_parameter.bounding_mode = ossia.BoundingMode.Clip
int_parameter.value = ossia.Value(100)                  ### TODO : int_parameter.value = 100
### TODO : int_parameter = int_node.create_parameter(type='Int', default=4, domain=[-66, 66], bounding='wrap', repetitions=True)
int_parameter.make_domain(ossia.Value(-10), ossia.Value(10))        ### TODO : int_parameter.make_domain(-10, 10)
int_parameter.apply_domain()

# create a node, create a float parameter, set its properties and initialize it
float_node = local_device.add_node("/test/value/float")
float_parameter = float_node.create_parameter(ossia.ValueType.Float)
### TODO : float_parameter = float_node.create_parameter(type='Float', default=0.2, domain=[0, 0.98], bounding='Clip', repetitions=False)
float_parameter.access_mode = ossia.AccessMode.Bi
float_parameter.bounding_mode = ossia.BoundingMode.Clip
float_parameter.value = ossia.Value(2.5)                  ### TODO : float_parameter.value = 2.5

float_parameter.make_domain(ossia.Value(-2.0), ossia.Value(2.0))      ### TODO : float_parameter.make_domain(-2.0, 2.0)
float_parameter.apply_domain()


# create a node, create a char parameter and initialize it
char_node = local_device.add_node("/test/value/char")
char_parameter = char_node.create_parameter(ossia.ValueType.Char)
### TODO : char_parameter = char_node.create_parameter(type='Char', default=z, domain=[a, z], bounding='fold', repetitions=True)
char_parameter.value = ossia.Value('a')


# create a node, create a string parameter and initialize it
string_node = local_device.add_node("/test/value/string")
string_parameter = string_node.create_parameter(ossia.ValueType.String)
### TODO : string_parameter = char_node.create_parameter(type='String', default='Hello world !', repetitions=True)
string_parameter.value= ossia.Value("hello world !")            ### TODO : string_parameter.value = "hello world !"


# create a node, create a 3 floats vector parameter and initialize it
vec3f_node = local_device.add_node("/test/value/vec3f")
vec3f_parameter = vec3f_node.create_parameter(ossia.ValueType.Vec3f)
### TODO : vec3f_parameter = vec3f_node.create_parameter(type='Vec3f', default=[0, 0, 0], repetitions=True)
vec3f_parameter.value = ossia.Value([100, 127, 255])            ### TODO : vec3f_parameter.value = [100, 127, 255]


# create a node, create a list parameter and initialize it
list_node = local_device.add_node("/test/value/list")
list_parameter = list_node.create_parameter(ossia.ValueType.List)
### TODO : list_parameter = list_node.create_parameter(type='List', default=[44100, "test.wav", 0.9], repetitions=True)
list_parameter.value = ossia.Value([ossia.Value(44100), ossia.Value("test.wav"), ossia.Value(0.9)]) ### TODO : list_parameter.value = [44100, "test.wav", 0.9]


# attach a callback function to the boolean parameter
def bool_value_callback(v):
  print(v)
bool_parameter.add_callback(bool_value_callback)

# attach a callback function to the integer parameter
def int_value_callback(v):
  print(v)
int_parameter.add_callback(int_value_callback)

# attach a callback function to the float parameter
def float_value_callback(v):
  print(v)
float_parameter.add_callback(float_value_callback)

# attach a callback function to the char parameter
def char_value_callback(v):
  print(v)
char_parameter.add_callback(char_value_callback)

# attach a callback function to the string parameter
def string_value_callback(v):
  print(v)
string_parameter.add_callback(string_value_callback)

# attach a callback function to the 3 floats vector parameter
def vec3f_value_callback(v):
  print(v)
vec3f_parameter.add_callback(vec3f_value_callback)

# attach a callback function to the list parameter
def list_value_callback(v):
  print(v)
list_parameter.add_callback(list_value_callback)



### LOCAL DEVICE EXPLORATION

# a function to iterate on node's tree recursively
def iterate_on_children(node):

  for child in node.children():
    if child.address:
      if child.address.__class__.__name__ == 'Parameter':
        print(str(child) + " " + str(child.address) + " <" + str(child.address.value_type) + ", " + str(child.address.access_mode) + ">")
        if child.address.have_domain():
          print("min : " + str(child.address.domain.min) + ", max : " + str(child.address.domain.max))
        else:
          print(child)
    iterate_on_children(child)

# iterate on our device
print("\nLOCAL DEVICE NAMESPACE\n")
iterate_on_children(local_device.root_node)




### REMOTE DEVICE SETUP

# try to connect to a remote device using OSCQuery protocol
oscquery_device = ossia.OSCQueryDevice("remoteDevice", "ws://127.0.0.1:5678", 9998)

# explore the remote device
oscquery_device.update()

# iterate on our device
print("\nREMOTE DEVICE NAMESPACE\n")
iterate_on_children(oscquery_device.root_node)




# MAIN LOOP
# wait and use i-score to change the value remotely
while True:
  time.sleep(1)
