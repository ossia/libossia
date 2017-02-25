#! /usr/bin/env python
import ossia_python as ossia
import time

print("OSSIA PYTHON LIBRARY EXAMPLE")

### LOCAL DEVICE SETUP

# create a device for this python program
local_device = ossia.LocalDevice("newDevice")

# enable OSCQuery communication for our device
local_device.create_oscquery_server(3456, 5678)

# create a node, create a boolean address and initialize it
bool_node = local_device.add_node("/test/value/bool")
bool_address = bool_node.create_address(ossia.ValueType.Bool)
bool_address.push_value(ossia.Value(True))

# create a node, create an integer address and initialize it
int_node = local_device.add_node("/test/value/int")
int_address = int_node.create_address(ossia.ValueType.Int)
int_address.push_value(ossia.Value(100))

# create a node, create a float address, set its properties and initialize it
float_node = local_device.add_node("/test/value/float")
float_address = float_node.create_address(ossia.ValueType.Float)
float_address.set_access_mode(ossia.AccessMode.Bi);
float_address.set_bounding_mode(ossia.BoundingMode.Clip);
float_address.get_domain().set_min(ossia.Value(-1.0));
float_address.get_domain().set_max(ossia.Value(1.0));
float_address.push_value(ossia.Value(1.5))								### BUG : the value is not clipped

# create a node, create a char address and initialize it
char_node = local_device.add_node("/test/value/char")
char_address = char_node.create_address(ossia.ValueType.Char)
char_address.push_value(ossia.Value('a'))

# create a node, create a string address and initialize it
string_node = local_device.add_node("/test/value/string")
string_address = string_node.create_address(ossia.ValueType.String)
string_address.push_value(ossia.Value("hello world !"))					### BUG : affiche juste h dans i-score

# create a node, create a 3 floats vector address and initialize it
vec3f_node = local_device.add_node("/test/value/vec3f")
vec3f_address = vec3f_node.create_address(ossia.ValueType.Vec3f)
vec3f_address.push_value(ossia.Value([100, 127, 255]))

# create a node, create a tuple address and initialize it
tuple_node = local_device.add_node("/test/value/tuple")
tuple_address = tuple_node.create_address(ossia.ValueType.Tuple)
tuple_value = ossia.Value([ossia.Value(44100), ossia.Value("test.wav"), ossia.Value(0.9)]) ### BUG : affiche juste le code ascii du string
tuple_address.push_value(tuple_value)

# attach a callback function to the boolean address
def bool_value_callback(v):
	print(v.get())
bool_address.add_callback(bool_value_callback)

# attach a callback function to the integer address
def int_value_callback(v):
	print(v.get())
int_address.add_callback(int_value_callback)

# attach a callback function to the float address
def float_value_callback(v):
	print(v.get())
float_address.add_callback(float_value_callback)

# attach a callback function to the char address
def char_value_callback(v):
	print(v.get())
char_address.add_callback(char_value_callback)

# attach a callback function to the string address
def string_value_callback(v):
	print(v.get())
string_address.add_callback(string_value_callback)

# attach a callback function to the 3 floats vector address
def vec3f_value_callback(v):
	print(v.get())
vec3f_address.add_callback(vec3f_value_callback)

# attach a callback function to the tuple address
def tuple_value_callback(v):
	print(v.get())
tuple_address.add_callback(tuple_value_callback)

### LOCAL DEVICE EXPLORATION

# a function to iterate on node's tree recursively
def iterate_on_children(node):

	for child in node.children():
		if child.get_address():
			print(str(child) + " " + str(child.get_address()))
		else:
			print(child)

		iterate_on_children(child)

# iterate on our device
print("\nLOCAL DEVICE NAMESPACE\n")
iterate_on_children(local_device.get_root_node())

### REMOTE DEVICE SETUP

try:
	# try to connect to a remote device using OSCQuery protocol
	oscquery_device = ossia.OSCQueryDevice("remoteDevice", "ws://10.0.1.83:5678", 9998)

	# explore the remote device
	oscquery_device.update()

	# iterate on our device
	print("\nREMOTE DEVICE NAMESPACE\n")
	iterate_on_children(oscquery_device.get_root_node())

except:
	print("\nfail to connect to ws://10.0.1.83:5678\n")

# MAIN LOOP

# wait and use i-score to change the value remotely
while True:
	time.sleep(1)

### TODO ###

# ADDRESS VALUE OPERATIONS
#v = address.clone_value() + ossia.Value(1)

# access address property
#type = address.type
#address.tags = ['test', 'demo']
#address.domain
#address.clipmode

#print(address.__dict__)