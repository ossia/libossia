cmake_minimum_required(VERSION 3.0)
project(APITests)

include_directories("${API_IMPL_FOLDER}/Jamoma/Sources")

find_package(Qt5 5.4 REQUIRED COMPONENTS Core Test)
find_package(Jamoma 0.6)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

function(addAPIQtTest TESTNAME TESTSRCS)
	if(${Jamoma_FOUND})
		add_executable(${TESTNAME}_Jamoma ${TESTSRCS})
		target_link_libraries(${TESTNAME}_Jamoma Qt5::Core Qt5::Test Jamoma::Modular Jamoma::Foundation APIJamoma Jamoma::Score)
		add_test(${TESTNAME}_Jamoma_target ${TESTNAME})
	endif()
#		add_executable(${TESTNAME}_Mock ${TESTSRCS})
#		target_link_libraries(${TESTNAME}_Mock Qt5::Core Qt5::Test Jamoma::Modular Jamoma::Foundation APIMock)
#		add_test(${TESTNAME}_Mock_target ${TESTNAME})

endFunction()

#addAPIQtTest(APIScenarioTest	    "${CMAKE_CURRENT_SOURCE_DIR}/ScenarioTest.cpp")
#addAPIQtTest(APITimeEventTest	    "${CMAKE_CURRENT_SOURCE_DIR}/TimeEventTest.cpp")

addAPIQtTest(APIDeviceTest	    "${CMAKE_CURRENT_SOURCE_DIR}/Network/DeviceTest.cpp")
addAPIQtTest(APINodeTest	    "${CMAKE_CURRENT_SOURCE_DIR}/Network/NodeTest.cpp")
addAPIQtTest(APIAddressTest	    "${CMAKE_CURRENT_SOURCE_DIR}/Network/AddressTest.cpp")

include(CTest)
