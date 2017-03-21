# OSSIA-PYTHON

ossia-python is a python binding of libossia
# Build tips

## build for python 3
`cmake .. -DPYTHON_EXECUTABLE=/usr/local/bin/python3 -DPYTHON_LIBRARY=/usr/local/opt/python3/Frameworks/Python.framework/Versions/3.6/lib/libpython3.6.dylib`

## build for python 2
`cmake .. -DPYTHON_EXECUTABLE=/usr/local/bin/python -DPYTHON_LIBRARY=/usr/local/opt/python/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib`

## build options
`OSSIA_PROTOCOL_HTTP:BOOL=ON`    
`OSSIA_PROTOCOL_MIDI:BOOL=ON`    
`OSSIA_PROTOCOL_OSCQUERY:BOOL=ON`    
`OSSIA_PROTOCOL_SERIAL:BOOL=OFF`    
`OSSIA_PROTOCOL_WEBSOCKETS:BOOL=OFF`    

# Work in progress
Network part of libossia is currently implemented (partially) in python.    
