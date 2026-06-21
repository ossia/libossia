ossia_use_system(use_sys Servus)
if(use_sys)
  find_package(Servus CONFIG GLOBAL)
endif()

if(NOT TARGET Servus_iface)
  add_subdirectory(3rdparty/Servus EXCLUDE_FROM_ALL)
endif()
