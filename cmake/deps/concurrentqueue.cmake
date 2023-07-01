if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(concurrentqueue 1.0 CONFIG GLOBAL)
endif()

if(NOT TARGET concurrentqueue::concurrentqueue)
  add_library(concurrentqueue INTERFACE IMPORTED GLOBAL)
  add_library(concurrentqueue::concurrentqueue ALIAS concurrentqueue)
  target_include_directories(concurrentqueue INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/concurrentqueue>")
endif()
