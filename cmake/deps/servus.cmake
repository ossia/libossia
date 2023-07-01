if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(Servus CONFIG GLOBAL)
endif()

if(NOT TARGET Servus_iface)
  add_subdirectory(3rdparty/Servus EXCLUDE_FROM_ALL)
endif()
