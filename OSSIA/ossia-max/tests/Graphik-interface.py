#! /usr/bin/env python
import ossia_python as ossia
import time

print("OSSIA PYTHON LIBRARY EXAMPLE")

### OSSIA DEVICE SETUP

# create a device for this python program
my_device = ossia.LocalDevice("newDevice")

# enable OSCQuery communication for our device
my_device.create_oscquery_server(3456, 5678)

# create a node, create a boolean address and initialize it
bool_node = my_device.add_node("/test/value/bool")
bool_address = bool_node.create_address(ossia.ValueType.Bool)
bool_address.push_value(ossia.Value(True))

# create a node, create an integer address and initialize it
int_node = my_device.add_node("/test/value/int")
int_address = int_node.create_address(ossia.ValueType.Int)
int_address.push_value(ossia.Value(100))

# create a node, create a float address, set its properties and initialize it
float_node = my_device.add_node("/test/value/float")
float_address = float_node.create_address(ossia.ValueType.Float)
float_address.set_access_mode(ossia.AccessMode.Bi);
float_address.set_bounding_mode(ossia.BoundingMode.Clip);
float_address.get_domain().set_min(ossia.Value(-1.0));
float_address.get_domain().set_max(ossia.Value(1.0));
float_address.push_value(ossia.Value(1.5))								### BUG : the value is not clipped

# create a node, create a char address and initialize it
char_node = my_device.add_node("/test/value/char")
char_address = char_node.create_address(ossia.ValueType.Char)
char_address.push_value(ossia.Value('a'))

# create a node, create a string address and initialize it
string_node = my_device.add_node("/test/value/string")
string_address = string_node.create_address(ossia.ValueType.String)
string_address.push_value(ossia.Value("hello world !"))					### BUG : affiche juste h dans i-score

# create a node, create a 3 floats vector address and initialize it
vec3f_node = my_device.add_node("/test/value/vec3f")
vec3f_address = vec3f_node.create_address(ossia.ValueType.Vec3f)
vec3f_address.push_value(ossia.Value([100, 127, 255]))

# create a node, create a tuple address and initialize it
tuple_node = my_device.add_node("/test/value/tuple")
tuple_address = tuple_node.create_address(ossia.ValueType.Tuple)
tuple_value = ossia.Value([ossia.Value(44100), ossia.Value("test.wav"), ossia.Value(0.9)]) ### BUG : affiche juste le code ascii du string
tuple_address.push_value(tuple_value)

# attach a callback function to the boolean address
def bool_value_callback(v):
	print(v.get_bool()) # TODO : have an unique .get() method

# attach a callback function to the integer address
def int_value_callback(v):
	print(v.get_int()) # TODO : have an unique .get() method

# attach a callback function to the float address
def float_value_callback(v):
	print(v.get_float()) # TODO : have an unique .get() method

# attach a callback function to the char address
def char_value_callback(v):
	print(v.get_char()) # TODO : have an unique .get() method

# attach a callback function to the string address
def string_value_callback(v):
	print(v.get_string()) # TODO : have an unique .get() method

# attach a callback function to the 3 floats vector address
def vec3f_value_callback(v):
	print(v.get_vec3f()) # TODO : have an unique .get() method

# attach a callback function to the tuple address
def tuple_value_callback(v):
	print(v.get()) # TODO : have an unique .get() method

bool_address.add_callback(bool_value_callback)
int_address.add_callback(int_value_callback)
float_address.add_callback(float_value_callback)
char_address.add_callback(char_value_callback)
string_address.add_callback(string_value_callback)
vec3f_address.add_callback(vec3f_value_callback)
tuple_address.add_callback(tuple_value_callback)

### LOCAL DEVICE EXPLORATION

# find an node and print it's address
#node = osc_device.find_node("/test/value/int")
#address = node.get_address()
#print(address)

# iterate on device's nodes
"""
def iterate_on_children(node):

	for child in node.get_children():
		address = child.get_address()
		if address != None:
			print(address)
		iterate_on_children(child)

iterate_on_children(my_device)
"""
# wait and use i-score to change the value remotely
while True:
	time.sleep(1)
	print("#")

### REMOTE DEVICE SETUP

# declare a remote client using OSC protocol
#osc_device = ossia.OSCClient("")
#osc_device.load_namespace("/file/path/...")

# ADDRESS VALUE OPERATIONS
#v = address.clone_value() + ossia.Value(1)

# access address property
#type = address.type
#address.tags = ['test', 'demo']
#address.domain
#address.clipmode

#print(address.__dict__)

### MISC

# export device namespace
#my_device.export_namespace('/Users/reno/Desktop/toto.bush')