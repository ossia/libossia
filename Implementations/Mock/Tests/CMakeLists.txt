cmake_minimum_required(VERSION 3.0)
project(APITests)

include_directories("${API_IMPL_FOLDER}/Jamoma/Sources")

find_package(Qt5 5.4 REQUIRED COMPONENTS Core Test)
find_package(Jamoma 0.6)

function(addAPIQtTest TESTNAME TESTSRCS)
		add_executable(${TESTNAME}_Jamoma ${TESTSRCS})
		add_executable(${TESTNAME}_Mock ${TESTSRCS})
		target_link_libraries(${TESTNAME}_Jamoma Qt5::Core Qt5::Test Jamoma::Modular Jamoma::Foundation APIJamoma)
		target_link_libraries(${TESTNAME}_Mock Qt5::Core Qt5::Test Jamoma::Modular Jamoma::Foundation APIMock)
		add_test(${TESTNAME}_Jamoma_target ${TESTNAME})
		add_test(${TESTNAME}_Mock_target ${TESTNAME})
endFunction()

#add_custom_target(ScenarioTestHeaders
#				  SOURCES
#				   "${CMAKE_CURRENT_SOURCE_DIR}/helpers/ForwardDeclaration.hpp"
#)

# Commands
addAPIQtTest(APIScenarioTest
			    "${CMAKE_CURRENT_SOURCE_DIR}/ScenarioTest.cpp")
addAPIQtTest(APITimeEventTest
			    "${CMAKE_CURRENT_SOURCE_DIR}/TimeEventTest.cpp")

addAPIQtTest(APIDeviceTest
			    "${CMAKE_CURRENT_SOURCE_DIR}/DeviceTest.cpp")

include(CTest)
