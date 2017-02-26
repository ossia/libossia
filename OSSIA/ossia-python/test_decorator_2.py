#! /usr/bin/env python
import ossia_python as ossia

# SOLUTION HERE !!! http://stackoverflow.com/questions/42461108/wrap-get-and-set-attributes-methods-without-editing-original-class

# create a device for this python program
local_device = ossia.LocalDevice("newDevice")

# enable OSCQuery communication for our device
local_device.create_oscquery_server(3456, 5678)

### TO PUT INTO an "ossia_decorator.py" file

# ossia decorator for property getter
def ossia_address_getter(original_getter):                      
    def new_getter(*args,**kwargs):
        print 'new_getter called'
        # TODO pull the value from the registered address
        if args and kwargs:
            v = original_getter(*args,**kwargs) # call the getter to do specific things on the current value
        else:
            original_getter()
        return v                                             
    return new_getter

 # ossia decorator for property setter
def ossia_address_setter(original_setter):                    
    def new_setter(*args,**kwargs):   
        print 'new_setter called' 
        if args and kwargs:   
            original_setter(*args,**kwargs) # call the setter to do specific things with the new value
        else:
            original_setter()
        # TODO push the value to the registered address                                                        
    return new_setter

 # ossia decorator for function
def ossia_address_function(original_function):                      
    def new_function(*args,**kwargs):
        print 'new_function called'
        if args and kwargs:
            original_function(*args,**kwargs) # call the function to do specific things with the new value 
        else:
            original_function()
        # TODO push the value to the registered address                                                        
    return new_function

# ossia decorator for class
def ossia_model(instanceToDecorate):
    print("ossia_model(instanceToDecorate)")

    # TODO: create a node for the instance
    import inspect
    import types
    for name, member in inspect.getmembers(instanceToDecorate):
        # filter all private accessors
        if name.startswith('_'):
            continue
        # is it a method ?
        if inspect.ismethod(member):
            print '%s: SET access mode' % name
            getattr(type(instanceToDecorate), name) = types.MethodType(ossia_address_function(getattr(type(instanceToDecorate), name)),instanceToDecorate)    
            getattr(type(instanceToDecorate), name)()
            continue
        # is it a property ?
        attr = getattr(type(instanceToDecorate), name)
        if isinstance(attr, property):
            # does the fset method exist ?
            if attr.fset is None:
                print '%s: GET access mode' % name
                #instanceToDecorate.attr.fget = ossia_address_function(attr.fget)
                #instanceToDecorate.attr.fget()
            else:
                print '%s: BI access mode' % name
                #attr.fget = new.instancemethod(ossia_address_getter(member), self.instanceToDecorate, None) # decoration
                #attr.fset = new.instancemethod(ossia_address_setter(member), self.instanceToDecorate, None) # decoration

                # TODO: create a node for each function and property

    return instanceToDecorate

### WHAT COULD BE IN ANY PYTHON LIBRARY

class Button(object):
    print("Button(object))")
    """
	Testing ossia python
    """
    def __init__(self, size=30):
        print("Button.__init__(self,*args,**kwargs)")

        #super(Button, self).__init__()
        self._size = size

    @property
    def size(self):
        print("Button.size(self)@property")
        return self._size
    @size.setter
    def size(self, size):
        print("Button.size(self)@width.setter")
        self._size = size

    def push(self):
        print("Button.push(self)")

    @property
    def area(self):
        print("Button.area(self)")
        return self._size * self._size

### WHAT TO CODE IN YOUR PROGRAM FILE

# decorate a class with ossia_model function
#print("\n*** Button class decoration process:\n")
#Button = ossia_model(Button)

print("\n*** Decorated Button instanciation process:\n")
my_button = ossia_model(Button())

print("\n*** Decorated Button use:\n")
my_button.member()
print my_button.size
my_button.size = 20
print my_button.area
my_button.push()
