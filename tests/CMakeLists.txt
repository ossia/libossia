### Setup ###
# Don't run tests on CI when we're building specific bindings...
if(OSSIA_CI)
  if(OSSIA_PD_ONLY OR OSSIA_MAX_ONLY OR OSSIA_PYTHON_ONLY OR OSSIA_NODEJS OR OSSIA_JAVA_ONLY OR OSSIA_UNITY3D)
    return()
  endif()
endif()

include_directories("${CMAKE_CURRENT_LIST_DIR}/catch")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
get_target_property(OSSIA_PROTOCOLS ossia OSSIA_PROTOCOLS)

### Tests ###
ossia_add_test(PresetTest   "${CMAKE_CURRENT_SOURCE_DIR}/Preset/PresetTest.cpp")
ossia_add_test(CueTest      "${CMAKE_CURRENT_SOURCE_DIR}/Preset/CueTest.cpp")
ossia_add_test(AddressTest  "${CMAKE_CURRENT_SOURCE_DIR}/Network/AddressTest.cpp")
ossia_add_test(PathTest     "${CMAKE_CURRENT_SOURCE_DIR}/Network/PathTest.cpp")
ossia_add_test(FilterTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/FilterTest.cpp")
ossia_add_test(FuzzyTest    "${CMAKE_CURRENT_SOURCE_DIR}/Network/FuzzyTest.cpp")
ossia_add_test(WSLoggerTest "${CMAKE_CURRENT_SOURCE_DIR}/Network/WSLogger.cpp")

if(NOT OSSIA_CI)
  # this test is too slow to run on CI
  ossia_add_test(DeviceTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/DeviceTest.cpp")
endif()


ossia_add_test(OSCTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCTest.cpp")

if(NOT WIN32)
  ossia_add_test(OSC_UnixTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_UnixTest.cpp")
  ossia_add_test(OSC_UDPTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_UDPTest.cpp")
  ossia_add_test(OSC_TCP_SlipTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_TCP_SlipTest.cpp")
  ossia_add_test(OSC_WS_Test   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_WS_Test.cpp")
  ossia_add_test(OSC_TCP_SizeTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_TCP_SizeTest.cpp")
  ossia_add_test(OSC_Unix_SlipTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_Unix_SlipTest.cpp")
  ossia_add_test(OSC_Unix_SizeTest   "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSC_Unix_SizeTest.cpp")
endif()

ossia_add_test(NodeTest     "${CMAKE_CURRENT_SOURCE_DIR}/Network/NodeTest.cpp")


ossia_add_test(ValueTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ValueTest.cpp")
ossia_add_test(DataspaceTest               "${CMAKE_CURRENT_SOURCE_DIR}/Editor/DataspaceTest.cpp")

if(MSVC AND NOT OSSIA_STATIC)
  add_custom_command(TARGET ossia_PresetTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ossia> $<TARGET_FILE_DIR:ossia_PresetTest>
  )
endif()

if(OSSIA_EDITOR)
  ossia_add_test(AutomationTest
      "${CMAKE_CURRENT_SOURCE_DIR}/Editor/AutomationTest.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TestUtils.hpp"
      )

  if(NOT OSSIA_CI)
    # this test is too reliant on specific hardware capabilities to run on travis
    ossia_add_test(ClockTest                 "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ClockTest.cpp")

    # TODO : investigate why this one does not pass on CI
    ossia_add_test(ScenarioAlgoTest            "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ScenarioAlgoTest.cpp")
    ossia_add_test(ScenarioTest                "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ScenarioTest.cpp")
  endif()

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
  ossia_add_test(MapperTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Editor/MapperTest.cpp")
  ossia_add_test(MessageTest                 "${CMAKE_CURRENT_SOURCE_DIR}/Editor/MessageTest.cpp")
  ossia_add_test(QuantificationTest          "${CMAKE_CURRENT_SOURCE_DIR}/Editor/QuantificationTest.cpp")
  ossia_add_test(StateTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Editor/StateTest.cpp")
  ossia_add_test(TimeIntervalTest            "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeIntervalTest.cpp")
  ossia_add_test(TimeEventTest               "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeEventTest.cpp")
  ossia_add_test(TimeSyncTest                "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeSyncTest.cpp")
  ossia_add_test(DataspaceMergeTest          "${CMAKE_CURRENT_SOURCE_DIR}/Editor/DataspaceMergeTest.cpp")
endif()

if(OSSIA_DATAFLOW)
  ossia_add_test(ValuePortTest               "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/ValuePortTest.cpp")
  ossia_add_test(DataflowTest                "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/DataflowTest.cpp")
  ossia_add_test(TickMethodTest              "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/TickMethodTest.cpp")
  ossia_add_test(TokenRequestTest            "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/TokenRequestTest.cpp")
  ossia_add_test(SoundTest                   "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/SoundTest.cpp")
  if(TARGET rubberband AND TARGET samplerate)
    target_link_libraries(ossia_SoundTest PRIVATE rubberband samplerate)
  endif()
endif()

if(OSSIA_QML)
  # The following lines are used to display the QMLs in the project view of IDEs
  SET(QMLS
    "${CMAKE_CURRENT_SOURCE_DIR}/testdata/http/http_example.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/testdata/qml/TestDevice.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/testdata/qml/TestDevice2.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/testdata/serial/serial_example.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/testdata/websocket/ws_example.qml"
    "${CMAKE_CURRENT_SOURCE_DIR}/testdata/websocket/ws_icmc.qml"
  )
  add_custom_target(ossia_test_ui_qml SOURCES ${QMLS})

  if(OSSIA_QML_DEVICE)
    ossia_add_test(QtTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QtDeviceTest.cpp")
    ossia_add_test(QmlTest                 "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QmlDeviceTest.cpp")
    ossia_add_test(QmlApiTest              "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QmlApiTest.cpp")
    ossia_add_test(QmlPresetTest           "${CMAKE_CURRENT_SOURCE_DIR}/Qt/QmlPresetTest.cpp")
    ossia_add_test(JSTest                  "${CMAKE_CURRENT_SOURCE_DIR}/Qt/JSTest.cpp")
  endif()

  if(OSSIA_PROTOCOL_WEBSOCKETS)
    ossia_add_test(WebSocketTest         "${CMAKE_CURRENT_SOURCE_DIR}/Network/WebSocketTest.cpp")
  endif()

  if(OSSIA_PROTOCOL_HTTP)
    ossia_add_test(HttpTest              "${CMAKE_CURRENT_SOURCE_DIR}/Network/HttpTest.cpp")
    ossia_add_test(HttpQMLTest              "${CMAKE_CURRENT_SOURCE_DIR}/Network/HttpQMLTest.cpp")
  endif()
endif()

if(OSSIA_PROTOCOL_OSCQUERY)
  # ossia_add_test(HttpRequestTest            "${CMAKE_CURRENT_SOURCE_DIR}/Network/HttpRequestTest.cpp")
  ossia_add_test(OSCQueryTest            "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryTest.cpp")
  ossia_add_test(OSCQueryDeviceTest      "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryDeviceTest.cpp")
  ossia_add_test(OSCQueryColorTest       "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryColorTest.cpp")
  ossia_add_test(OSCQueryDenseTest       "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryDenseTest.cpp")
  if(OSSIA_CPP)
    ossia_add_test(OSCQueryTreeCallbackTest  "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryTreeCallbackTest.cpp")
    ossia_add_test(OSCQueryValueCallbackTest "${CMAKE_CURRENT_SOURCE_DIR}/Network/OSCQueryValueCallbackTest.cpp")
  endif()
  ossia_add_test(MultiplexTest            "${CMAKE_CURRENT_SOURCE_DIR}/Network/MultiplexTest.cpp")
  ossia_add_test(OSCQueryRename "Network/OSCQuery_rename.cpp")
endif()

if(OSSIA_PROTOCOL_MIDI)
  ossia_add_test(MIDITest             "${CMAKE_CURRENT_SOURCE_DIR}/Network/MIDITest.cpp")
endif()

if(OSSIA_PROTOCOL_MINUIT)
  ossia_add_test(MinuitTest             "${CMAKE_CURRENT_SOURCE_DIR}/Network/MinuitTest.cpp")
endif()

if(OSSIA_PROTOCOL_PHIDGETS)
  ossia_add_test(PhidgetTest             "${CMAKE_CURRENT_SOURCE_DIR}/Network/PhidgetTest.cpp")
endif()

if(OSSIA_CPP)
  ossia_add_test(Cpp98Test               "${CMAKE_CURRENT_SOURCE_DIR}/CPP/CPP98.cpp")
endif()

if(OSSIA_C)
  ossia_add_test(devices_c  "${CMAKE_CURRENT_SOURCE_DIR}/C/unit-tests/unit-tests-devices.cpp")
  ossia_add_test(api_c      "${CMAKE_CURRENT_SOURCE_DIR}/C/unit-tests/unit-tests-c-api.cpp")
  ossia_add_test(presets_c  "${CMAKE_CURRENT_SOURCE_DIR}/C/unit-tests/unit-tests-presets.cpp")
endif()

### Benchmarks ###

if(OSSIA_BENCHMARKS)
  find_package(benchmark REQUIRED)
  function(ossia_add_bench TESTNAME TESTSRCS)
    add_executable(ossia_${TESTNAME} ${TESTSRCS})
    target_compile_options(ossia_${TESTNAME} PUBLIC ${OSSIA_COMPILE_OPTIONS})
    if(UNIX)
      target_compile_options(ossia_${TESTNAME} PUBLIC -O3 -march=native)
    endif()

    target_link_libraries(ossia_${TESTNAME} PUBLIC ${OSSIA_LINK_OPTIONS} ossia benchmark::benchmark Catch2::Catch2WithMain)

    # add_test(NAME ossia_target_${TESTNAME} COMMAND ossia_${TESTNAME})
  endFunction()

  if(OSSIA_DATAFLOW)
    if(TARGET ${QT_PREFIX}::Core)
      ossia_add_bench(GraphBenchmark              "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/GraphBenchmark.cpp")
      target_link_libraries(ossia_GraphBenchmark PRIVATE ${QT_PREFIX}::Core)
    endif()

    if(TARGET TBB::TBB)
      ossia_add_bench(TBBBenchmark                "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/TestTBB.cpp")
      target_link_libraries(TBBBenchmark PRIVATE TBB::TBB)
    endif()

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
    ossia_add_bench(CPPTFBenchmark              "${CMAKE_CURRENT_SOURCE_DIR}/Dataflow/TestCPPTF.cpp")
    ossia_add_bench(MixNSines                   "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/MixNSines.cpp")
  endif()

  ossia_add_bench(DeviceBenchmark             "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/Random.hpp")
  ossia_add_bench(DeviceBenchmark_Nsec_client "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_Nsec_client.cpp")
  ossia_add_bench(DeviceBenchmark_Nsec_server "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_Nsec_server.cpp")
  ossia_add_bench(DeviceBenchmark_client      "${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks/DeviceBenchmark_client.cpp")
endif()

# A command to copy the test data.
if(MSVC)
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug/")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release/")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../src/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../src/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../src/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml")

  file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml/Ossia/qmldir-testing" "${CMAKE_CURRENT_BINARY_DIR}/Debug/testdata/qml/Ossia/qmldir")
  file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml/Ossia/qmldir-testing" "${CMAKE_CURRENT_BINARY_DIR}/Release/testdata/qml/Ossia/qmldir")
  file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir-testing" "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir")
else()
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/testdata" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../src/ossia-qt/Ossia" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml")

  file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir-testing"
    "${CMAKE_CURRENT_BINARY_DIR}/testdata/qml/Ossia/qmldir")
endif()

include(CTest)
