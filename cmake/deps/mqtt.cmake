if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(async-mqtt5 CONFIG GLOBAL)
endif()

if(NOT TARGET Async::MQTT5)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/async-mqtt5" async-mqtt5)
endif()

