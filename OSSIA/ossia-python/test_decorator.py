#! /usr/bin/env python
import ossia_python as ossia

# SOLUTION HERE !!! http://stackoverflow.com/questions/42461108/wrap-get-and-set-attributes-methods-without-editing-original-class

# create a device for this python program
local_device = ossia.LocalDevice("newDevice")

# enable OSCQuery communication for our device
local_device.create_oscquery_server(3456, 5678)

### TO PUT INTO an "ossia_decorator.py" file

# ossia decorator for property getter
def ossia_address_getter(o):                      
    print 'new_getter called'
    print type(o)                                                  

 # ossia decorator for property setter
def ossia_address_setter(original_setter):                      
    def new_setter(*args,**kwargs):   
        print 'new_setter called'      
        original_setter(*args,**kwargs) # call the setter to do specific things with the new value 
        # TODO push the value to the registered address                                                        
    return new_setter

 # ossia decorator for function
def ossia_address_function(original_function):                      
    def new_function(*args,**kwargs):
        print 'new_function called'         
        original_function(*args,**kwargs) # call the function to do specific things with the new value 
        # TODO push the value to the registered address                                                        
    return new_function

# ossia decorator for class
def ossia_model(ClassToDecorate):
    print("ossia_model(ClassToDecorate)")

    class DecoratedClass(object):
        print("DecoratedClass(object))")

        def __init__(self,*args,**kwargs):

            self.instanceToDecorate = ClassToDecorate(*args,**kwargs)

            # TODO: create a node for the instance
            import inspect
            for name, member in inspect.getmembers(self.instanceToDecorate):
                # filter all private accessors
                if name.startswith('_'):
                    continue
                # is it a method ?
                if inspect.ismethod(member):
                    print '%s: SET access mode' % name
                    member = ossia_address_function(member) # decoration
                    continue
                # is it a property ?
                attr = getattr(type(self.instanceToDecorate), name, None)
                if isinstance(attr, property):
                    # does the fset method exist ?
                    if attr.fset is None:
                        print '%s: GET access mode' % name
                        member = ossia_address_getter(member) # decoration
                    else:
                        print '%s: BI access mode' % name
                        member = ossia_address_getter(member) # decoration
                        member = ossia_address_setter(member) # decoration

                # TODO: create a node for each function and property

        def __getattribute__(self,s):
            """
            this is called whenever any attribute of a DecoratedClass object is accessed. This function first tries to 
            get the attribute off DecoratedClass. If it fails then it tries to fetch the attribute from self.instanceToDecorate (an
            instance of the decorated class). If it manages to fetch the attribute from self.oInstance, and 
            the attribute is an instance method then `time_this` is applied.
            """
            try:    
                x = super(DecoratedClass,self).__getattribute__(s)
            except AttributeError:      
                pass
            else:
                return x

            return self.instanceToDecorate.__getattribute__(s)

    return DecoratedClass

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
    @ossia_address_getter
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
print("\n*** Button class decoration process:\n")

print("\n*** Decorated Button instanciation process:\n")
my_button = Button()

print("\n*** Decorated Button use:\n")
print my_button.size
my_button.size = 20
print my_button.area
my_button.push()
