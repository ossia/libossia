if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(tuplet CONFIG GLOBAL)
endif()

if(NOT TARGET tuplet::tuplet)
  add_library(tuplet INTERFACE IMPORTED GLOBAL)
  add_library(tuplet::tuplet ALIAS tuplet)
  target_include_directories(tuplet INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/tuplet/include>")
endif()
