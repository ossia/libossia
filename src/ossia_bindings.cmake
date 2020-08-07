# Bindings
if(OSSIA_JAVA)
  add_subdirectory(ossia-java)
endif()

if(OSSIA_UNITY3D)
  add_subdirectory(ossia-unity3d)
endif()

if(OSSIA_NODEJS)
  add_subdirectory(ossia-node)
endif()

if(OSSIA_PYTHON)
  add_subdirectory(ossia-python ossia-python/pyossia)
endif()

if(OSSIA_PD)
  add_subdirectory(ossia-pd)
endif()

if(OSSIA_MAX)
  add_subdirectory(ossia-max)
endif()
