# Build tips

* First do a git clone according to the procedure here: https://github.com/OSSIA/libossia/wiki/Building
* Then go to the root libossia folder
* Create a build folder and go inside
* Run one of the following commands:


## For Python 3
`cmake .. -DPYTHON_EXECUTABLE=/usr/local/bin/python3 -DPYTHON_LIBRARY=/usr/local/opt/python3/Frameworks/Python.framework/Versions/3.6/lib/libpython3.6.dylib -DOSSIA_PYTHON=1`

## For Python 2
`cmake .. -DPYTHON_EXECUTABLE=/usr/local/bin/python -DPYTHON_LIBRARY=/usr/local/opt/python/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib -DOSSIA_PYTHON=1`

## build options
`OSSIA_PROTOCOL_HTTP:BOOL=ON`    
`OSSIA_PROTOCOL_MIDI:BOOL=ON`    
`OSSIA_PROTOCOL_OSCQUERY:BOOL=ON`    
`OSSIA_PROTOCOL_SERIAL:BOOL=OFF`    
`OSSIA_PROTOCOL_WEBSOCKETS:BOOL=OFF`   
