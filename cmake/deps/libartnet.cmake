if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(artnet CONFIG GLOBAL)
endif()

if(NOT TARGET artnet::artnet)
  set(LIBARTNET_INSTALL "${OSSIA_INSTALL_STATIC_DEPENDENCIES}" CACHE INTERNAL "" FORCE)
  set(LIBARTNET_DIR "${OSSIA_3RDPARTY_FOLDER}/libartnet")
  add_subdirectory("${LIBARTNET_DIR}" libartnet)
endif()

