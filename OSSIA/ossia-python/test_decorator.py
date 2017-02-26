#! /usr/bin/env python
import ossia_python as ossia

# create a device for this python program
local_device = ossia.LocalDevice("newDevice")

# enable OSCQuery communication for our device
local_device.create_oscquery_server(3456, 5678)

### TO PUT INTO an "ossia_decorator.py" file

# ossia decorator for property getter
def ossia_address_getter(original_getter):                      
    def new_getter(*args,**kwargs):
        # TODO pull the value from the registered address 
        v = original_getter(*args,**kwargs) # call the getter to do specific things on the current value        
        return v                                             
    return new_getter

 # ossia decorator for property setter
def ossia_address_setter(original_setter):                      
    def new_setter(*args,**kwargs):         
        original_setter(*args,**kwargs) # call the setter to do specific things with the new value 
        # TODO push the value to the registered address                                                        
    return new_setter

 # ossia decorator for function
def ossia_address_function(original_function):                      
    def new_function(*args,**kwargs):         
        original_function(*args,**kwargs) # call the function to do specific things with the new value 
        # TODO push the value to the registered address                                                        
    return new_function

# ossia decorator for class
def ossia_model(ClassToDecorate):
    print("ossia_model(ClassToDecorate)")

    class DecoratedClass(object):
        print("DecoratedClass(object))")

        def __init__(self,*args,**kwargs):
            print("DecoratedClass.__init__(self,*args,**kwargs)")

            self.oInstance = ClassToDecorate(*args,**kwargs)

            # TODO HERE ? create a node for the instance ?

            # TODO HERE ? create a node for each property ?
            # TODO HERE ? look for property getters and setters to decorate them ?

            # TODO HERE ? create a node for each function ?
            # TODO HERE ? look for functions to decorate them ?
        """
        def __getattribute__(self,s):
            print("DecoratedClass.__getattribute__(self,s)")
            
            # this is called whenever any attribute of a DecoratedClass object is accessed. This function first tries to 
            # get the attribute off DecoratedClass. If it fails then it tries to fetch the attribute from self.oInstance (an
            # instance of the decorated class). If it manages to fetch the attribute from self.oInstance, and 
            # the attribute is an instance method then the appropriate decorators (ossia_address_getter and/or ossia_address_setter) are applied.
            
            try: 
                x = super(DecoratedClass,self).__getattribute__(s)
            except AttributeError:      
                pass
            else:
                return x

            x = self.oInstance.__getattribute__(s)

            if type(x) == type(self.__init__): # it is an instance method
                return ossia_address_setter(x) # this is equivalent of just decorating the method with ossia_address_setter
            else:
                return x
        """
    return DecoratedClass

### WHAT COULD BE IN ANY PYTHON LIBRARY

class Button(object):
    print("Button(object))")
    """
	Testing ossia python
    """
    def __init__(self, width=30, height=10):
        print("Button.__init__(self,*args,**kwargs)")

        #super(Button, self).__init__()
        self._width = width
        self.height = height

    @property
    def width(self):
        print("Button.width(self)@property")
        return self._width
    @width.setter
    def width(self, width):
        print("Button.width(self)@width.setter")
        self._width = width

    def push(self):
        print("Button.push(self)")

    @property
    def area(self):
        print("Button.area(self)")
        return self.width * self.height

### WHAT TO CODE IN YOUR PROGRAM FILE

# decorate a class with ossia_model function
print("\n*** Button class decoration process:\n")
Button = ossia_model(Button)

print("\n*** Decorated Button instanciation process:\n")
my_button = Button()

print("\n*** Explore Decorated Button instance:\n")
print(type(my_button.width))
