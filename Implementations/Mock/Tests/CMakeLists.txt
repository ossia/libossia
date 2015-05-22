cmake_minimum_required(VERSION 3.0)
project(APITests)

#include_directories("${ISCORE_ROOT_SOURCE_DIR}/base")

find_package(Qt5 5.4 REQUIRED COMPONENTS Core Test)

function(addIScoreQtTest TESTNAME TESTSRCS)
		add_executable(${TESTNAME} ${TESTSRCS})
		target_link_libraries(${TESTNAME} Qt5::Core Qt5::Test)
		add_test(${TESTNAME}_target ${TESTNAME})
endFunction()

#add_custom_target(ScenarioTestHeaders
#				  SOURCES
#				   "${CMAKE_CURRENT_SOURCE_DIR}/helpers/ForwardDeclaration.hpp"
#)

# Commands
addIScoreQtTest(APIScenarioTest
			    "${CMAKE_CURRENT_SOURCE_DIR}/ScenarioTest.cpp")
addIScoreQtTest(APITimeEventTest
			    "${CMAKE_CURRENT_SOURCE_DIR}/TimeEventTest.cpp")

addIScoreQtTest(APIDeviceTest
			    "${CMAKE_CURRENT_SOURCE_DIR}/DeviceTest.cpp")

include(CTest)
