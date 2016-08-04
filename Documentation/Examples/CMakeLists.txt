function(ossia_add_example TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME}_example ${TESTSRCS})
    target_link_libraries(ossia_${TESTNAME}_example ossia)
endFunction()

ossia_add_example(editor_curve "${CMAKE_CURRENT_SOURCE_DIR}/Editor/curve.cpp")
ossia_add_example(editor_expression "${CMAKE_CURRENT_SOURCE_DIR}/Editor/expression.cpp")
ossia_add_example(editor_scenario "${CMAKE_CURRENT_SOURCE_DIR}/Editor/scenario.cpp")
ossia_add_example(editor_state "${CMAKE_CURRENT_SOURCE_DIR}/Editor/state.cpp")

ossia_add_example(network_minuit_exploration "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_exploration.cpp")
ossia_add_example(network_minuit_publication "${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication.cpp")
ossia_add_example(network_osc "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC.cpp")
