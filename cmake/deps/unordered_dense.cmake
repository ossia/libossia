if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(unordered_dense CONFIG GLOBAL)
endif()

if(NOT TARGET unordered_dense::unordered_dense)
  add_library(unordered_dense INTERFACE IMPORTED GLOBAL)
  add_library(unordered_dense::unordered_dense ALIAS unordered_dense)
  target_include_directories(unordered_dense INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/unordered_dense/include>")
endif()
