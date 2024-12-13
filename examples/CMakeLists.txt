function(ossia_add_example TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME}_example ${TESTSRCS})

    target_compile_options(ossia_${TESTNAME}_example PUBLIC ${OSSIA_COMPILE_OPTIONS})
    target_link_libraries(ossia_${TESTNAME}_example PUBLIC ossia)
endFunction()


if(OSSIA_DATAFLOW)
    ossia_add_example(minidaw "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/MiniDAW.cpp")

    if(OSSIA_PROTOCOL_OSCQUERY)
      ossia_add_example(editor_graph "${CMAKE_CURRENT_SOURCE_DIR}/Editor/example_graph.cpp")
      ossia_add_example(mixsines "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/MixSines.cpp")
      ossia_add_example(mixsinesaddr "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/MixSinesAddress.cpp")
    endif()

    if(OSSIA_PROTOCOL_OSC)
      ossia_add_example(synthosc "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/SynthOSC.cpp")
    endif()

    if(OSSIA_EDITOR)
      ossia_add_example(editor_curve "${CMAKE_CURRENT_SOURCE_DIR}/Editor/curve.cpp")
      ossia_add_example(editor_expression "${CMAKE_CURRENT_SOURCE_DIR}/Editor/expression.cpp")
      ossia_add_example(editor_scenario "${CMAKE_CURRENT_SOURCE_DIR}/Editor/scenario.cpp")
      ossia_add_example(editor_automation "${CMAKE_CURRENT_SOURCE_DIR}/Editor/automation.cpp")

      if(OSSIA_PROTOCOL_OSCQUERY)
        ossia_add_example(editor_state "${CMAKE_CURRENT_SOURCE_DIR}/Editor/state.cpp")
      endif()
    endif()
endif()


if(OSSIA_PROTOCOL_MINUIT)
  ossia_add_example(network_minuit_exploration "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_exploration.cpp")
  ossia_add_example(network_minuit_publication "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication.cpp")
  ossia_add_example(network_minuit_publication_large "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication_large.cpp")
endif()

if(OSSIA_PROTOCOL_OSC)
  ossia_add_example(network_osc "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC.cpp")
  ossia_add_example(network_osc_echo "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_echo.cpp")
  ossia_add_example(network_osc_send "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_send.cpp")
  ossia_add_example(network_rate_limiter "${CMAKE_CURRENT_SOURCE_DIR}/Network/RateLimiter.cpp")

  if(NOT WIN32)
    ossia_add_example(network_osc_unix_echo "${CMAKE_CURRENT_SOURCE_DIR}/Network/unix/OSC_echo.cpp")
    ossia_add_example(network_osc_unix_send "${CMAKE_CURRENT_SOURCE_DIR}/Network/unix/OSC_send.cpp")

    ossia_add_example(network_osc_playback "${CMAKE_CURRENT_SOURCE_DIR}/Network/asio/OSC_playback.cpp")
  endif()

  ossia_add_example(network_osc_ws_echo "${CMAKE_CURRENT_SOURCE_DIR}/Network/websockets/OSC_echo.cpp")
  ossia_add_example(network_osc_ws_send "${CMAKE_CURRENT_SOURCE_DIR}/Network/websockets/OSC_send.cpp")

  ossia_add_example(network_osc_serial_send "${CMAKE_CURRENT_SOURCE_DIR}/Network/serial/OSC_send.cpp")
  ossia_add_example(network_osc_udp_send_receive "${CMAKE_CURRENT_SOURCE_DIR}/Network/udp/OSC_send_receive.cpp")
endif()

add_subdirectory("Qt/DeviceBuilder")

if(OSSIA_PROTOCOL_OSCQUERY)
  ossia_add_example(network_oscquery_publication "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_publication.cpp")
  ossia_add_example(network_oscquery_publication_simple "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_publication_simple.cpp")
  ossia_add_example(network_oscquery_exploration "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_exploration.cpp")
  ossia_add_example(network_oscquery_preset "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_preset.cpp")
  ossia_add_example(network_oscquery_various "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_various.cpp")
  ossia_add_example(network_oscquery_bridge "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_bridge.cpp")
  ossia_add_example(network_oscquery_validator "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_validator.cpp")
  ossia_add_example(network_oscquery_exploration_asio "${CMAKE_CURRENT_SOURCE_DIR}/Network/asio/OSCQuery_exploration_asio.cpp")
  ossia_add_example(network_oscquery_publication_asio "${CMAKE_CURRENT_SOURCE_DIR}/Network/asio/OSCQuery_publication_asio.cpp")
  ossia_add_example(network_oscquery_recorder "${CMAKE_CURRENT_SOURCE_DIR}/Network/asio/OSCQuery_recorder.cpp")

  ossia_add_example(pd_controller "${CMAKE_CURRENT_SOURCE_DIR}/Network/PdController.cpp")

  if(OSSIA_PROTOCOL_LIBMAPPER)
    ossia_add_example(network_libmapper "${CMAKE_CURRENT_SOURCE_DIR}/Network/Libmapper.cpp")
  endif()

  if(OSSIA_QT)
    find_package(${QT_VERSION} COMPONENTS Widgets)
    ossia_add_example(qt_colors "${CMAKE_CURRENT_SOURCE_DIR}/Qt/Colors/main.cpp")
    target_link_libraries(ossia_qt_colors_example PRIVATE ${QT_PREFIX}::Widgets)
  endif()
  #ossia_add_example(preset_device_example "${CMAKE_CURRENT_SOURCE_DIR}/Preset/DeviceExample.cpp")
  #ossia_add_example(preset_parsing_example "${CMAKE_CURRENT_SOURCE_DIR}/Preset/ParsingExample.cpp")
  ossia_add_example(preset_print_namespace "${CMAKE_CURRENT_SOURCE_DIR}/Preset/PrintNamespace.cpp")

  if(NOT "${CMAKE_BUILD_TYPE}" MATCHES "Debug")
    if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "GLIBCXX_DEBUG")
      find_package(OpenCV
        COMPONENTS
        core
        videoio
        imgcodecs
        imgproc)
      if(OpenCV_FOUND)
        ossia_add_example(jpeg_streamer "${CMAKE_CURRENT_SOURCE_DIR}/Web/JpegStreamer.cpp")
        target_link_libraries(ossia_jpeg_streamer_example PRIVATE ${OpenCV_LIBS})

        ossia_add_example(double_ws_server "${CMAKE_CURRENT_SOURCE_DIR}/Web/DoubleWSServer.cpp")
        target_link_libraries(ossia_double_ws_server_example PRIVATE ${OpenCV_LIBS})
      endif()
    endif()
  endif()
endif()

if(OSSIA_PROTOCOL_JOYSTICK)
  ossia_add_example(joystick "${CMAKE_CURRENT_SOURCE_DIR}/Network/joystick/joystick.cpp")
endif()

if(OSSIA_PROTOCOL_SERIAL)
  ossia_add_example(network_arduino "${CMAKE_CURRENT_SOURCE_DIR}/Network/Arduino.cpp")
endif()

if(OSSIA_PROTOCOL_MQTT5)
  ossia_add_example(mqtt_publish "${CMAKE_CURRENT_SOURCE_DIR}/Network/Mqtt_publication.cpp")
endif()

ossia_add_example(export_namespace_to_json "${CMAKE_CURRENT_SOURCE_DIR}/Preset/Export_namespace_to_json.cpp")
ossia_add_example(logger "${CMAKE_CURRENT_SOURCE_DIR}/Common/Logger.cpp")
ossia_add_example(fuzzysearch "${CMAKE_CURRENT_SOURCE_DIR}/Common/FuzzySearch.cpp")
