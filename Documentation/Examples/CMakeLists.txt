file(GLOB_RECURSE SRCS *.cpp)

find_package(Jamoma 0.6)

function(addAPIExample TESTNAME TESTSRCS)
        if(${Jamoma_FOUND})
                add_executable(API_${TESTNAME}_example ${TESTSRCS})
                target_link_libraries(API_${TESTNAME}_example APIJamoma)
        endif()

endFunction()

#addAPIExample(branching	    "${CMAKE_CURRENT_SOURCE_DIR}/branching.cpp")
#addAPIExample(complete		    "${CMAKE_CURRENT_SOURCE_DIR}/complete.cpp")
addAPIExample(curve		    "${CMAKE_CURRENT_SOURCE_DIR}/curve.cpp")
addAPIExample(network	    	    "${CMAKE_CURRENT_SOURCE_DIR}/network.cpp")
addAPIExample(scenario	    	    "${CMAKE_CURRENT_SOURCE_DIR}/scenario.cpp")
addAPIExample(state		    "${CMAKE_CURRENT_SOURCE_DIR}/state.cpp")
