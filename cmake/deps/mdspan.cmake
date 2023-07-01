if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(mdspan CONFIG GLOBAL)
endif()

if(NOT TARGET mdspan::mdspan)
  add_library(mdspan INTERFACE IMPORTED GLOBAL)
  add_library(mdspan::mdspan ALIAS mdspan)
  target_include_directories(mdspan INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/mdspan/include>")
endif()
