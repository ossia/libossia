cmake_minimum_required(VERSION 3.0)
project(ossia_tests_C)

function(ossia_add_test_c TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME}_test ${TESTSRCS})
    target_link_libraries(ossia_${TESTNAME}_test ossia)
    add_test(NAME ossia_target_${TESTNAME}_test COMMAND ossia_${TESTNAME}_test)
endFunction()

ossia_add_test_c(devices_c "${CMAKE_CURRENT_SOURCE_DIR}/unit-tests/unit-tests-devices.cpp")
ossia_add_test_c(api_c "${CMAKE_CURRENT_SOURCE_DIR}/unit-tests/unit-tests-c-api.cpp")
ossia_add_test_c(presets_c "${CMAKE_CURRENT_SOURCE_DIR}/unit-tests/unit-tests-presets.cpp")
