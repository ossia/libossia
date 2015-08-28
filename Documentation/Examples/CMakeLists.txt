file(GLOB_RECURSE SRCS *.cpp)

find_package(Jamoma 0.6)

function(addAPIExample TESTNAME TESTSRCS)
        if(${Jamoma_FOUND})
                add_executable(API_${TESTNAME}_example ${TESTSRCS})
                target_link_libraries(API_${TESTNAME}_example APIJamoma)
        endif()

endFunction()

addAPIExample(editor_curve			"${CMAKE_CURRENT_SOURCE_DIR}/Editor/curve.cpp")
addAPIExample(editor_expression			"${CMAKE_CURRENT_SOURCE_DIR}/Editor/expression.cpp")
addAPIExample(editor_scenario			"${CMAKE_CURRENT_SOURCE_DIR}/Editor/scenario.cpp")
addAPIExample(editor_state			"${CMAKE_CURRENT_SOURCE_DIR}/Editor/state.cpp")

addAPIExample(network_minuit_exploration	"${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_exploration.cpp")
addAPIExample(network_minuit_publication	"${CMAKE_CURRENT_SOURCE_DIR}/Network/Minuit_publication.cpp")
addAPIExample(network_osc                       "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC.cpp")
