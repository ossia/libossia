# All tests defined in this file are based on Qt Framework
find_package(Qt5 5.6 REQUIRED COMPONENTS Core Test Qml)

if(Qt5_VERSION VERSION_GREATER_EQUAL 5.11.0
  AND "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
  return() # Qt > 5.11 doesn't ship 32bit binary anymore
endif()


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
get_target_property(OSSIA_PROTOCOLS ossia OSSIA_PROTOCOLS)

function(ossia_add_test TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME} ${TESTSRCS})
    target_compile_options(ossia_${TESTNAME} PUBLIC ${OSSIA_COMPILE_OPTIONS})
    target_include_directories(ossia_${TESTNAME} PUBLIC ${OSSIA_3RDPARTY_FOLDER}/verdigris/src)
    target_link_libraries(ossia_${TESTNAME} PUBLIC ${OSSIA_LINK_OPTIONS} Qt5::Core Qt5::Test Qt5::Qml ossia)

    if(OSSIA_SANITIZE)
        sanitize_build(ossia_${TESTNAME})
    endif()
    use_gold(ossia_${TESTNAME})

    add_test(NAME ossia_target_${TESTNAME} COMMAND ossia_${TESTNAME})
endFunction()

if(OSSIA_EDITOR)
    ossia_add_test(AutomationTest
        "${CMAKE_CURRENT_SOURCE_DIR}/Editor/AutomationTest.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TestUtils.hpp"
        )

    if(NOT OSSIA_CI)
        # this test is too reliant on specific hardware capabilities to run on travis
        ossia_add_test(ClockTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ClockTest.cpp")
    endif()
endif()

if(OSSIA_QT)
# The following lines are used to display the QMLs in the project view of IDEs
SET(QMLS "${CMAKE_CURRENT_SOURCE_DIR}/testdata/http/http_example.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/qml/TestDevice.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/qml/TestDevice2.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/serial/serial_example.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/websocket/ws_example.qml"
         "${CMAKE_CURRENT_SOURCE_DIR}/testdata/websocket/ws_icmc.qml")
add_custom_target(ossia_test_ui_qml SOURCES ${QMLS})
endif()

if(OSSIA_EDITOR)
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
ossia_add_test(ScenarioAlgoTest            "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ScenarioAlgoTest.cpp")
ossia_add_test(StateTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/StateTest.cpp")
ossia_add_test(TimeIntervalTest            "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeIntervalTest.cpp")
ossia_add_test(TimeEventTest               "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeEventTest.cpp")
ossia_add_test(TimeSyncTest                "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeSyncTest.cpp")
ossia_add_test(ValueTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ValueTest.cpp")
ossia_add_test(DataspaceTest               "${CMAKE_CURRENT_SOURCE_DIR}/Editor/DataspaceTest.cpp")
ossia_add_test(DataspaceMergeTest          "${CMAKE_CURRENT_SOURCE_DIR}/Editor/DataspaceMergeTest.cpp")
endif()

if(OSSIA_DATAFLOW)
ossia_add_test(DataflowTest                "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/DataflowTest.cpp")
ossia_add_test(TickMethodTest              "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/TickMethodTest.cpp")
endif()

ossia_add_test(PresetTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Preset/PresetTest.cpp")
ossia_add_test(AddressTest                 "${CMAKE_CURRENT_SOURCE_DIR}/Network/AddressTest.cpp")
ossia_add_test(PathTest                    "${CMAKE_CURRENT_SOURCE_DIR}/Network/PathTest.cpp")

if(OSSIA_QT) #for http_device and sanitize_name(QString...)
    ossia_add_test(DeviceTest              "${CMAKE_CURRENT_SOURCE_DIR}/Network/DeviceTest.cpp")
    ossia_add_test(NodeTest                "${CMAKE_CURRENT_SOURCE_DIR}/Network/NodeTest.cpp")
    ossia_add_test(QtTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QtDeviceTest.cpp")
    ossia_add_test(QmlTest                 "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QmlDeviceTest.cpp")
    ossia_add_test(QmlApiTest              "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QmlApiTest.cpp")
    ossia_add_test(QmlPresetTest           "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QmlPresetTest.cpp")
    ossia_add_test(JSTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Qt/JSTest.cpp")
endif()


if("${OSSIA_PROTOCOLS}" MATCHES "WebSockets")
  ossia_add_test(WebSocketTest              "${CMAKE_CURRENT_SOURCE_DIR}/Network/WebSocketTest.cpp")
endif()
if("${OSSIA_PROTOCOLS}" MATCHES "OSCQuery")
  ossia_add_test(OSCQueryTest               "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryTest.cpp")
  ossia_add_test(OSCQueryColorTest          "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryColorTest.cpp")
endif()
if("${OSSIA_PROTOCOLS}" MATCHES "HTTP")
  ossia_add_test(HttpTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Network/HttpTest.cpp")
endif()
if(OSSIA_PROTOCOL_PHIDGETS)
  ossia_add_test(PhidgetTest                "${CMAKE_CURRENT_SOURCE_DIR}/Network/PhidgetTest.cpp")
endif()
if(OSSIA_CPP)
  ossia_add_test(Cpp98Test                  "${CMAKE_CURRENT_SOURCE_DIR}/CPP/CPP98.cpp")
endif()

if(OSSIA_C)
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
endif()


if(OSSIA_BENCHMARKS)
############### Benchmarks ################
function(ossia_add_bench TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME} ${TESTSRCS})
    target_compile_options(ossia_${TESTNAME} PUBLIC ${OSSIA_COMPILE_OPTIONS})
    if(UNIX)
      target_compile_options(ossia_${TESTNAME} PUBLIC -O3 -march=native)
    endif()

    target_link_libraries(ossia_${TESTNAME} PUBLIC ${OSSIA_LINK_OPTIONS} Qt5::Core Qt5::Test ossia)

    if(OSSIA_SANITIZE)
        sanitize_build(ossia_${TESTNAME})
    endif()
    use_gold(ossia_${TESTNAME})

    # add_test(NAME ossia_target_${TESTNAME} COMMAND ossia_${TESTNAME})
endFunction()

ossia_add_test(IntervalBenchmark            "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/IntervalBenchmark.cpp")
ossia_add_bench(AutomationBenchmark         "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/AutomationBenchmark.cpp")
ossia_add_bench(AutomationFloatBenchmark    "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/AutomationFloatBenchmark.cpp")
ossia_add_bench(AutomationFloatDataBench    "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/AutomationFloatDataBench.cpp")
ossia_add_bench(MappingGluttonBenchmark     "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MappingGluttonBenchmark.cpp")
ossia_add_bench(MappingStrictBenchmark      "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MappingStrictBenchmark.cpp")
ossia_add_bench(MappingStrictDataBenchmark  "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MappingStrictDataBenchmark.cpp")
ossia_add_bench(MappingGluttonDataBenchmark "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MappingGluttonDataBenchmark.cpp")
ossia_add_bench(MappingBenchmark            "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MappingBenchmark.cpp")
ossia_add_bench(MappingDataBenchmark        "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MappingDataBenchmark.cpp")
ossia_add_bench(OverallBenchmark            "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/OverallBenchmark.cpp")
ossia_add_bench(GraphBenchmark              "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/GraphBenchmark.cpp")
ossia_add_bench(DeviceBenchmark             "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark.cpp"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/Random.hpp")
ossia_add_bench(DeviceBenchmark_Nsec_client "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_Nsec_client.cpp")
ossia_add_bench(DeviceBenchmark_Nsec_server "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_Nsec_server.cpp")
ossia_add_bench(DeviceBenchmark_client      "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_client.cpp")

ossia_add_bench(MixNSines                   "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MixNSines.cpp")
endif()

# A command to copy the test data.
if(MSVC)
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug/")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release/")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../OSSIA/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../OSSIA/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../OSSIA/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml")

    file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml/Ossia/qmldir-testing" "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml/Ossia/qmldir")
    file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml/Ossia/qmldir-testing" "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml/Ossia/qmldir")
    file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir-testing" "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir")
else()
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../OSSIA/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml")

    file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir-testing"
        "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir")
endif()

include(CTest)
