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


# create a node, create a boolean address and initialize it
bool_node = local_device.add_node("/test/value/bool")
bool_address = bool_node.create_address(ossia.ValueType.Bool)

bool_address.access_mode = ossia.AccessMode.Get
bool_address.value = ossia.Value(True)									### TODO : bool_address.value = True


# create a node, create an integer address and initialize it
int_node = local_device.add_node("/test/value/int")
int_address = int_node.create_address(ossia.ValueType.Int)

int_address.access_mode = ossia.AccessMode.Set
int_address.bounding_mode = ossia.BoundingMode.Clip
int_address.value = ossia.Value(100)									### TODO : int_address.value = 100

int_address.make_domain(ossia.Value(-10), ossia.Value(10))				### TODO : int_address.make_domain(-10, 10)
int_address.apply_domain()

# create a node, create a float address, set its properties and initialize it
float_node = local_device.add_node("/test/value/float")
float_address = float_node.create_address(ossia.ValueType.Float)

float_address.access_mode = ossia.AccessMode.Bi
float_address.bounding_mode = ossia.BoundingMode.Clip
float_address.value = ossia.Value(2.5)									### TODO : float_address.value = 2.5

float_address.make_domain(ossia.Value(-2.0), ossia.Value(2.0))			### TODO : float_address.make_domain(-2.0, 2.0)
float_address.apply_domain()


# create a node, create a char address and initialize it
char_node = local_device.add_node("/test/value/char")
char_address = char_node.create_address(ossia.ValueType.Char)

char_address.value = ossia.Value('a')


# create a node, create a string address and initialize it
string_node = local_device.add_node("/test/value/string")
string_address = string_node.create_address(ossia.ValueType.String)

string_address.value= ossia.Value("hello world !")						### TODO : string_address.value = "hello world !"


# create a node, create a 3 floats vector address and initialize it
vec3f_node = local_device.add_node("/test/value/vec3f")
vec3f_address = vec3f_node.create_address(ossia.ValueType.Vec3f)

vec3f_address.value = ossia.Value([100, 127, 255])						### TODO : vec3f_address.value = [100, 127, 255]


# create a node, create a tuple address and initialize it
tuple_node = local_device.add_node("/test/value/tuple")
tuple_address = tuple_node.create_address(ossia.ValueType.Tuple)

tuple_address.value = ossia.Value([ossia.Value(44100), ossia.Value("test.wav"), ossia.Value(0.9)]) ### TODO : tuple_address.value = [44100, "test.wav", 0.9]


# attach a callback function to the boolean address
def bool_value_callback(v):
	print(v)
bool_address.add_callback(bool_value_callback)

# attach a callback function to the integer address
def int_value_callback(v):
	print(v)
int_address.add_callback(int_value_callback)

# attach a callback function to the float address
def float_value_callback(v):
	print(v)
float_address.add_callback(float_value_callback)

# attach a callback function to the char address
def char_value_callback(v):
	print(v)
char_address.add_callback(char_value_callback)

# attach a callback function to the string address
def string_value_callback(v):
	print(v)
string_address.add_callback(string_value_callback)

# attach a callback function to the 3 floats vector address
def vec3f_value_callback(v):
	print(v)
vec3f_address.add_callback(vec3f_value_callback)

# attach a callback function to the tuple address
def tuple_value_callback(v):
	print(v)
tuple_address.add_callback(tuple_value_callback)

### LOCAL DEVICE EXPLORATION

# a function to iterate on node's tree recursively
def iterate_on_children(node):

	for child in node.children():
		if child.address:
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

try:
	# try to connect to a remote device using OSCQuery protocol
	oscquery_device = ossia.OSCQueryDevice("remoteDevice", "ws://127.0.0.1:5678", 9998)

	# explore the remote device
	oscquery_device.update()

	# iterate on our device
	print("\nREMOTE DEVICE NAMESPACE\n")
	iterate_on_children(oscquery_device.get_root_node())

except:
	print("\nfail to connect\n")

# MAIN LOOP

# wait and use i-score to change the value remotely
while True:
	time.sleep(1)

### TODO ###

# ADDRESS VALUE OPERATIONS
#  

#print(address.__dict__)
