if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(verdigris CONFIG GLOBAL)
endif()

if(NOT TARGET verdigris::verdigris)
  add_library(verdigris INTERFACE IMPORTED GLOBAL)
  add_library(verdigris::verdigris ALIAS verdigris)
  target_include_directories(verdigris INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/verdigris/src>")
endif()
