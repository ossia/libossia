if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(dno CONFIG GLOBAL)
endif()

if(NOT TARGET dno::dno)
  add_library(dno INTERFACE IMPORTED GLOBAL)
  add_library(dno::dno ALIAS dno)
  target_include_directories(dno INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/dno>")
endif()
