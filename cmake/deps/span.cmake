if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(span CONFIG GLOBAL)
endif()

if(NOT TARGET span::span)
  add_library(span INTERFACE IMPORTED GLOBAL)
  add_library(span::span ALIAS span)
  target_include_directories(span INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/span/include>")
endif()
