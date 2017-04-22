find_package(Qt5 5.6 REQUIRED COMPONENTS Core Test)
if(OSSIA_NO_QT)
    return()
endif()
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

get_target_property(OSSIA_PROTOCOLS ossia OSSIA_PROTOCOLS)

function(ossia_add_test TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME} ${TESTSRCS})
    target_compile_options(ossia_${TESTNAME} PUBLIC ${OSSIA_COMPILE_OPTIONS})
    target_link_libraries(ossia_${TESTNAME} PUBLIC ${OSSIA_LINK_OPTIONS} Qt5::Core Qt5::Test ossia)

    if(OSSIA_SANITIZE)
        sanitize_build(ossia_${TESTNAME})
    endif()
    use_gold(ossia_${TESTNAME})

    add_test(NAME ossia_target_${TESTNAME} COMMAND ossia_${TESTNAME})
endFunction()

ossia_add_test(AutomationTest
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/AutomationTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TestUtils.hpp"
    )

if(NOT OSSIA_CI)
    # this test is too reliant on specific hardware capabilities to run on travis
    ossia_add_test(ClockTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ClockTest.cpp")
endif()

# The following lines are used to display the QMLs in the project view of IDEs
SET(QMLS "${CMAKE_CURRENT_SOURCE_DIR}/testdata/http/http_example.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/qml/TestDevice.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/qml/TestDevice2.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/serial/serial_example.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/websocket/ws_example.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/websocket/ws_icmc.qml")
add_custom_target(ossia_test_ui_qml SOURCES ${QMLS})

ossia_add_test(CurveTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/CurveTest.cpp")
ossia_add_test(CurveSegmentEmptyTest       "${CMAKE_CURRENT_SOURCE_DIR}/Editor/CurveSegment/CurveSegmentEmptyTest.cpp")
ossia_add_test(CurveSegmentLinearTest      "${CMAKE_CURRENT_SOURCE_DIR}/Editor/CurveSegment/CurveSegmentLinearTest.cpp")
ossia_add_test(CurveSegmentPowerTest       "${CMAKE_CURRENT_SOURCE_DIR}/Editor/CurveSegment/CurveSegmentPowerTest.cpp")
ossia_add_test(DomainTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Editor/DomainTest.cpp")
ossia_add_test(ExpressionTest              "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionTest.cpp")
ossia_add_test(ExpressionAtomTest          "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionAtomTest.cpp")
ossia_add_test(ExpressionCompositionTest   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionCompositionTest.cpp")
ossia_add_test(ExpressionNotTest           "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionNotTest.cpp")
ossia_add_test(ExpressionPulseTest         "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionPulseTest.cpp")
ossia_add_test(LoopTest                    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/LoopTest.cpp")
ossia_add_test(MapperTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Editor/MapperTest.cpp")
ossia_add_test(MessageTest                 "${CMAKE_CURRENT_SOURCE_DIR}/Editor/MessageTest.cpp")
ossia_add_test(ScenarioTest                "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ScenarioTest.cpp")
ossia_add_test(StateTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/StateTest.cpp")
ossia_add_test(TimeConstraintTest          "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeConstraintTest.cpp")
ossia_add_test(TimeEventTest               "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeEventTest.cpp")
ossia_add_test(TimeNodeTest                "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeNodeTest.cpp")
ossia_add_test(TimeValueTest               "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeValueTest.cpp")
ossia_add_test(ValueTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ValueTest.cpp")
ossia_add_test(DataspaceTest               "${CMAKE_CURRENT_SOURCE_DIR}/Editor/DataspaceTest.cpp")
ossia_add_test(DataspaceMergeTest          "${CMAKE_CURRENT_SOURCE_DIR}/Editor/DataspaceMergeTest.cpp")

ossia_add_test(DataflowTest                "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/DataflowTest.cpp")

ossia_add_test(PresetTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Preset/PresetTest.cpp")

ossia_add_test(AddressTest                 "${CMAKE_CURRENT_SOURCE_DIR}/Network/AddressTest.cpp")
ossia_add_test(DeviceTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Network/DeviceTest.cpp")
ossia_add_test(NodeTest                    "${CMAKE_CURRENT_SOURCE_DIR}/Network/NodeTest.cpp")
ossia_add_test(PathTest                    "${CMAKE_CURRENT_SOURCE_DIR}/Network/PathTest.cpp")

ossia_add_test(QtTest                      "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QtDeviceTest.cpp")
ossia_add_test(QmlTest                     "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QmlDeviceTest.cpp")

if("${OSSIA_PROTOCOLS}" MATCHES "WebSockets")
  ossia_add_test(WebSocketTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Network/WebSocketTest.cpp")
endif()
if("${OSSIA_PROTOCOLS}" MATCHES "OSCQuery")
  ossia_add_test(OSCQueryTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryTest.cpp")
endif()

if(OSSIA_BENCHMARKS)
ossia_add_test(DeviceBenchmark_Nsec_client "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_Nsec_client.cpp")
ossia_add_test(DeviceBenchmark_Nsec_server "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_Nsec_server.cpp")
ossia_add_test(DeviceBenchmark_client      "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_client.cpp")
ossia_add_test(DeviceBenchmark             "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark.cpp"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/Random.hpp")
# ossia_add_test(AutomationBenchmark         "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/AutomationBenchmark.cpp
endif()


function(ossia_add_test_c TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME}_test ${TESTSRCS})

    target_compile_options(ossia_${TESTNAME}_test PUBLIC ${OSSIA_COMPILE_OPTIONS})
    target_link_libraries(ossia_${TESTNAME}_test PUBLIC ${OSSIA_LINK_OPTIONS} ossia)

    if(OSSIA_SANITIZE)
        sanitize_build(ossia_${TESTNAME}_test)
    endif()
    use_gold(ossia_${TESTNAME}_test)

    add_test(NAME ossia_target_${TESTNAME}_test COMMAND ossia_${TESTNAME}_test)
endFunction()

ossia_add_test_c(devices_c "${CMAKE_CURRENT_SOURCE_DIR}/C/unit-tests/unit-tests-devices.cpp")
ossia_add_test_c(api_c "${CMAKE_CURRENT_SOURCE_DIR}/C/unit-tests/unit-tests-c-api.cpp")
ossia_add_test_c(presets_c "${CMAKE_CURRENT_SOURCE_DIR}/C/unit-tests/unit-tests-presets.cpp")

# A command to copy the test data.

if(MSVC)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug/")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release/")

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../OSSIA/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../OSSIA/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml")

    file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml/Ossia/qmldir"
                "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml/Ossia/qmldir")
else()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../OSSIA/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml")

    file(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir")
endif()

include(CTest)
