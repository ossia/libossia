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
ossia_add_example(editor_automation "${CMAKE_CURRENT_SOURCE_DIR}/Editor/automation.cpp")

ossia_add_example(network_minuit_exploration "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_exploration.cpp")
ossia_add_example(network_minuit_publication "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication.cpp")
ossia_add_example(network_minuit_publication_large "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication_large.cpp")
ossia_add_example(network_osc "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC.cpp")
ossia_add_example(network_oscquery_publication "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_publication.cpp")
ossia_add_example(network_oscquery_exploration "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQuery_exploration.cpp")
ossia_add_example(network_osc_echo "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_echo.cpp")
ossia_add_example(network_osc_send "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_send.cpp")


#ossia_add_example(preset_device_example "${CMAKE_CURRENT_SOURCE_DIR}/Preset/DeviceExample.cpp")
#ossia_add_example(preset_parsing_example "${CMAKE_CURRENT_SOURCE_DIR}/Preset/ParsingExample.cpp")
ossia_add_example(preset_print_namespace "${CMAKE_CURRENT_SOURCE_DIR}/Preset/PrintNamespace.cpp")
ossia_add_example(export_namespace_to_json "${CMAKE_CURRENT_SOURCE_DIR}/Preset/Export_namespace_to_json.cpp")
ossia_add_example(logger "${CMAKE_CURRENT_SOURCE_DIR}/Common/Logger.cpp")
