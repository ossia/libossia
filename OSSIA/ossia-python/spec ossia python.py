#! /usr/bin/env python
import ossia_python as ossia
import time

### LOCAL DEVICE SETUP

# create a device for this python program
my_device = ossia.LocalDevice("newDevice")
#my_device.author = "ossia team"
#my_device.version = "0.1.0"

# enable Minuit communication for our device
my_device.asMinuitServer("127.0.0.1", 9998, 13579)

# create a node and create an integer address
node = my_device.addNode("/test/value")
address = node.createAddress(1) # TODO : ossia.Value.INT

# initialize the value
address.pushValue(ossia.Value(100))
print(ossia.value_to_pretty_string(address.cloneValue()))

# setup a callback function that prints any changes
def test_value_callback(v):
	print(ossia.value_to_pretty_string(address.cloneValue()))

address.addCallback(test_value_callback)

# use i-score to change the value remotely
while True:
	time.sleep(1)
	print("#")

### REMOTE DEVICE TREE

# declare a remote device using OSC protocol
"""
osc_device = ossia.OSCDevice("")
osc_device.loadNamespace("/file/path/...")

# find an node
node = osc_device.findNode("/foo/bar")
address = node.getAddress()
"""
# iterate on device's nodes
"""
def iterate_on_children(node):

	for child in node.getChildren():
		address = child.getAddress()
		if address != None:
			print(address)
		iterate_on_children(child)

iterate_on_children(osc_device)
"""
# ADDRESS VALUE OPERATIONS
#v = address.cloneValue() + ossia.Value(1)

# access address property
"""
type = address.type
address.tags = ['test', 'demo']
address.domain
address.clipmode

print(address.__dict__)
"""
### MISC

# export device namespace
"""my_device.exportNamespace('/Users/reno/Desktop/toto.bush')
"""