if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(oscpack 1.2 CONFIG GLOBAL)
endif()

# add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)


if(NOT TARGET oscpack::oscpack)
  add_library(oscpack INTERFACE IMPORTED GLOBAL)
  add_library(oscpack::oscpack ALIAS oscpack)
  target_include_directories(oscpack INTERFACE $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/oscpack>)
endif()
