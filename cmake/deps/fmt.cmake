if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(fmt 10 CONFIG GLOBAL)
endif()

if(NOT TARGET fmt::fmt)
  set(OSSIA_FMT_INTERNAL 1 CACHE "" INTERNAL)

  add_library(fmt INTERFACE IMPORTED GLOBAL)
  add_library(fmt::fmt ALIAS fmt)
  target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY=1)
  target_include_directories(fmt INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/fmt/include>")
endif()

set(OSSIA_HAS_FMT 1 CACHE INTERNAL "")
