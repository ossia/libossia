function(ossia_add_example TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME}_example ${TESTSRCS})

    target_compile_options(ossia_${TESTNAME}_example PUBLIC ${OSSIA_COMPILE_OPTIONS})
    target_link_libraries(ossia_${TESTNAME}_example PUBLIC ossia)

    if(OSSIA_SANITIZE)
        sanitize_build(ossia_${TESTNAME}_example)
    endif()
    use_gold(ossia_${TESTNAME}_example)

endFunction()

ossia_add_example(editor_curve "${CMAKE_CURRENT_SOURCE_DIR}/Editor/curve.cpp")
ossia_add_example(editor_expression "${CMAKE_CURRENT_SOURCE_DIR}/Editor/expression.cpp")
ossia_add_example(editor_scenario "${CMAKE_CURRENT_SOURCE_DIR}/Editor/scenario.cpp")
ossia_add_example(editor_state "${CMAKE_CURRENT_SOURCE_DIR}/Editor/state.cpp")

ossia_add_example(network_minuit_exploration "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_exploration.cpp")
ossia_add_example(network_minuit_publication "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication.cpp")
ossia_add_example(network_minuit_publication_large "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication_large.cpp")
ossia_add_example(network_osc "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC.cpp")
ossia_add_example(network_oscquery_publication "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_publication.cpp")
ossia_add_example(network_oscquery_exploration "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_exploration.cpp")
target_include_directories(ossia_network_oscquery_publication_example PRIVATE
  ${OSSIA_3RDPARTY_FOLDER}/asio/asio/include
  ${OSSIA_3RDPARTY_FOLDER}/websocketpp
  )
target_include_directories(ossia_network_oscquery_exploration_example PRIVATE
  ${OSSIA_3RDPARTY_FOLDER}/asio/asio/include
  ${OSSIA_3RDPARTY_FOLDER}/websocketpp
  )

#ossia_add_example(preset_device_example "${CMAKE_CURRENT_SOURCE_DIR}/Preset/DeviceExample.cpp")
#ossia_add_example(preset_parsing_example "${CMAKE_CURRENT_SOURCE_DIR}/Preset/ParsingExample.cpp")
ossia_add_example(preset_print_namespace "${CMAKE_CURRENT_SOURCE_DIR}/Preset/PrintNamespace.cpp")
ossia_add_example(export_namespace_to_json "${CMAKE_CURRENT_SOURCE_DIR}/Preset/Export_namespace_to_json.cpp")
