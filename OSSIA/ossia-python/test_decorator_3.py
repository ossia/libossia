#! /usr/bin/env python
import ossia_python as ossia

# SOLUTION HERE !!! http://stackoverflow.com/questions/42461108/wrap-get-and-set-attributes-methods-without-editing-original-class

# create a device for this python program
local_device = ossia.LocalDevice("newDevice")

# ossia decorator for class
def ossia_class(original_class):
    print 'decorating class'
    return original_class

# ossia decorator for init
def ossia_init(original_init):
    print 'decorating init'
    def new_init(*args,**kwargs):
        if kwargs:
            print 'new_init called with args and kargs'
            original_init(*args,**kwargs)
        else:
            print 'new_init called with args'
            original_init(*args)                                          
    return new_init

# ossia decorator for property
def ossia_property(original_getter):
    print 'decorating getter'
    def new_getter(*args,**kwargs):
        # TODO pull the value from the registered address
        if kwargs:
            print 'new_getter called with args and kargs'
            v = original_getter(*args,**kwargs) # call the getter to do specific things on the current value
        else:
            print 'new_getter called with args'
            v = original_getter(*args)
        return v                                             
    return new_getter

# ossia decorator for property setter
def ossia_setter(original_setter):
    print 'decorating setter' 
    def new_setter(*args,**kwargs):   
        if kwargs:
            print 'new_setter called with args and kargs'
            original_setter(*args,**kwargs) # call the setter to do specific things with the new value
        else:
            print 'new_setter called with args'
            original_setter(*args)
        # TODO push the value to the registered address
    return new_setter

# ossia decorator for function
def ossia_function(original_function):
    print 'decorating function'
    def new_function(*args,**kwargs):
        if kwargs:
            print 'new_function called with args and kargs'     
            original_function(*args,**kwargs) # call the function to do specific things with the new value
        else:
            print 'new_function called with args'
            original_function(*args)
        # TODO push the value to the registered address                                                        
    return new_function

class OssiaAddress:
    print 'OssiaAddress(object)'

    def __init__(self):
        print 'OssiaAddress.__init__(self)'
        super(OssiaAddress, self).__init__()

    def ossia_address_method(self):
        print 'ossia_address_method(self)'

### WHAT COULD BE IN ANY PYTHON LIBRARY

@ossia_class
class Button(object, OssiaAddress):
    print 'Button(object)'
    """
	Testing ossia python
    """
    @ossia_init
    def __init__(self, size=30):
        print 'Button.__init__(self, size)'
        super(Button, self).__init__()
        self._size = size

    @property
    @ossia_property
    def size(self):
        print 'Button.size(self)@property'
        return self._size
    @size.setter
    @ossia_setter
    def size(self, size):
        print 'Button.size(self)@width.setter'
        self._size = size

    @ossia_function
    def push(self):
        print 'Button.push(self)'

    @property
    @ossia_property
    def area(self):
        print 'Button.area(self)'
        return self._size * self._size

### WHAT TO CODE IN YOUR PROGRAM FILE

# enable OSCQuery communication for our device
local_device.create_oscquery_server(3456, 5678)

print '\n*** Decorated Button use:\n'
my_button = Button(40)

print my_button.size
my_button.size = 20
print my_button.area
my_button.push()
