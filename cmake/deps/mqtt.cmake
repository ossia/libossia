if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(async-mqtt5 CONFIG GLOBAL)
endif()

if(NOT TARGET Async::MQTT5)
    add_library(async_mqtt5 INTERFACE IMPORTED GLOBAL)
    target_include_directories(async_mqtt5
      INTERFACE
        "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/async-mqtt5/include>"
    )

    if(TARGET Boost::boost)
      target_link_libraries(async_mqtt5 INTERFACE $<BUILD_INTERFACE:Boost::boost>)
    elseif(TARGET boost)
      target_link_libraries(async_mqtt5 INTERFACE $<BUILD_INTERFACE:boost>)
    endif()

    add_library(Async::MQTT5 ALIAS async_mqtt5)
endif()

