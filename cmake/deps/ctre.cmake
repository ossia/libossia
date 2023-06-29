if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(ctre 3.7 CONFIG GLOBAL)
endif()

if(NOT TARGET ctre::ctre)
  add_library(ctre INTERFACE IMPORTED GLOBAL)
  add_library(ctre::ctre ALIAS ctre)
  target_include_directories(ctre INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/compile-time-regular-expressions/include>")
endif()

set(OSSIA_HAS_CTRE 1 CACHE INTERNAL "")
