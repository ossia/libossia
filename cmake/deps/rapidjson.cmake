if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(RapidJSON 1.2 CONFIG GLOBAL)
endif()

if(NOT TARGET rapidjson::rapidjson)
  add_library(rapidjson INTERFACE IMPORTED GLOBAL)
  add_library(rapidjson::rapidjson ALIAS rapidjson)
  target_include_directories(rapidjson INTERFACE $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/rapidjson/include>)
endif()
