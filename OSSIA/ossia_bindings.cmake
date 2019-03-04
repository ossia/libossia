# Bindings
if(OSSIA_JAVA)
  add_subdirectory(ossia-java)
endif()

if(OSSIA_NODEJS)
  add_subdirectory(ossia-node)
endif()

if(NOT WIN32)
if(OSSIA_PYTHON)
  add_subdirectory(ossia-python ossia-python/pyossia)
endif()
endif()

if(OSSIA_PD)
  add_subdirectory(ossia-pd)
endif()

if(OSSIA_MAX)
  add_subdirectory(ossia-max)
endif()
