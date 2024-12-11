cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(ossia C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(OSSIA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(OSSIA_3RDPARTY_FOLDER ${OSSIA_ROOT}/3rdparty)
set(SRC ${OSSIA_ROOT}/src/ossia)
include(${OSSIA_ROOT}/src/ossia_sources.cmake)

configure_file(${OSSIA_ROOT}/src/ossia-config.hpp.in ossia-config.hpp)

add_library(ossia)
target_compile_options(ossia PUBLIC -std=c++23 -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables)
target_compile_definitions(ossia PUBLIC
    OSSIA_EXPORTS=1
    OSSIA_FREESTANDING=1
    OSSIA_NO_SMALLVECTOR=1
    OSSIA_EDITOR=1
    OSSIA_SCENARIO_DATAFLOW=1
    LIBREMIDI_HEADER_ONLY=1
)
target_link_libraries(ossia PUBLIC -Wl,-z,defs -fuse-ld=mold)
target_include_directories(ossia PUBLIC /home/jcelerier/ossia/score/3rdparty/libossia/3rdparty/boost_1_86_0)

target_include_directories(ossia SYSTEM PUBLIC
  ${OSSIA_ROOT}/src
  #$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/compile-time-regular-expressions/include>
  $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include>
  $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/tuplet/include>
  $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include>
  $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/span/include>
  $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/unordered_dense/include>
  $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/libremidi/include>
  $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/concurrentqueue>
)

set(SOURCE_FILES

"${SRC}/context.cpp"
"${SRC}/detail/thread.cpp"
"${SRC}/detail/any_map.cpp"

"${SRC}/audio/audio_parameter.cpp"
"${SRC}/audio/audio_device.cpp"
"${SRC}/audio/audio_engine.cpp"

"${SRC}/network/base/device.cpp"
"${SRC}/network/base/protocol.cpp"
"${SRC}/network/base/parameter.cpp"
"${SRC}/network/base/name_validation.cpp"
"${SRC}/network/base/node.cpp"
"${SRC}/network/base/node_attributes.cpp"
"${SRC}/network/base/node_functions.cpp"
"${SRC}/network/common/complex_type.cpp"
"${SRC}/network/common/path.cpp"
"${SRC}/network/base/osc_address.cpp"
"${SRC}/network/common/extended_types.cpp"
"${SRC}/network/common/value_mapping.cpp"
"${SRC}/network/dataspace/dataspace_visitors.cpp"
"${SRC}/network/dataspace/detail/dataspace_impl.cpp"
"${SRC}/network/domain/domain_base.cpp"
"${SRC}/network/domain/detail/domain_impl.cpp"
"${SRC}/network/domain/clamp.cpp"
"${SRC}/network/domain/clamp_min.cpp"
"${SRC}/network/domain/clamp_max.cpp"
"${SRC}/network/domain/wrap.cpp"
"${SRC}/network/domain/fold.cpp"
"${SRC}/network/exceptions.cpp"
"${SRC}/network/value/value.cpp"

"${SRC}/dataflow/data.cpp"
"${SRC}/dataflow/port.cpp"
"${SRC}/dataflow/graph_node.cpp"
"${SRC}/dataflow/graph/graph.cpp"
"${SRC}/dataflow/execution_state.cpp"
"${SRC}/dataflow/execution/execution_policy.cpp"
"${SRC}/dataflow/execution/default_policy.cpp"
"${SRC}/dataflow/execution/merged_policy.cpp"
"${SRC}/dataflow/execution/local_state_execution_policy.cpp"
"${SRC}/dataflow/nodes/state.cpp"

"${SRC}/editor/curve/curve.cpp"

"${SRC}/editor/exceptions.cpp"
"${SRC}/editor/expression/expression.cpp"
"${SRC}/editor/expression/expression_bool.cpp"
"${SRC}/editor/expression/expression_composition.cpp"
"${SRC}/editor/expression/expression_generic.cpp"
"${SRC}/editor/expression/expression_not.cpp"
"${SRC}/editor/expression/expression_pulse.cpp"
"${SRC}/editor/expression/expression_atom.cpp"

"${SRC}/editor/state/message.cpp"
"${SRC}/editor/state/state.cpp"
"${SRC}/editor/state/state_element.cpp"

"${SRC}/editor/scenario/scenario.cpp"
"${SRC}/editor/scenario/time_interval.cpp"
"${SRC}/editor/scenario/time_event.cpp"
"${SRC}/editor/scenario/time_process.cpp"
"${SRC}/editor/scenario/time_sync.cpp"
"${SRC}/editor/scenario/detail/scenario_sync_musical_execution.cpp"
"${SRC}/editor/scenario/detail/scenario_offset.cpp"
"${SRC}/editor/scenario/detail/scenario_sync_execution.cpp"
"${SRC}/editor/scenario/detail/scenario_execution.cpp"
)

add_executable(test_minimal test.cpp)
target_link_libraries(test_minimal PRIVATE ossia)

target_sources(ossia PRIVATE ${SOURCE_FILES})

install(DIRECTORY "${SRC}" DESTINATION include
        FILES_MATCHING PATTERN "*.h")
install(DIRECTORY "${SRC}" DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY "${SRC}" DESTINATION src
        FILES_MATCHING PATTERN "*.h")
install(DIRECTORY "${SRC}" DESTINATION src
        FILES_MATCHING PATTERN "*.hpp")


# install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include/" DESTINATION src)
# install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/tuplet/include/" DESTINATION src)
# install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include/" DESTINATION src)
# install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/span/include/" DESTINATION src)
# install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/unordered_dense/include/" DESTINATION src)
# install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/libremidi/include/" DESTINATION src)
# install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/concurrentqueue/" DESTINATION src
#     FILES_MATCHING
#     PATTERN "*.h"
#     PATTERN "*/build*" EXCLUDE
#     PATTERN "*/tests*" EXCLUDE
#     PATTERN "*/benchmarks*" EXCLUDE
# )
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/boost_1_86_0/boost" DESTINATION include)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include/" DESTINATION include)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/tuplet/include/" DESTINATION include)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include/" DESTINATION include)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/span/include/" DESTINATION include)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/unordered_dense/include/" DESTINATION include)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/libremidi/include/" DESTINATION include)
install(DIRECTORY "${OSSIA_3RDPARTY_FOLDER}/concurrentqueue/" DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*/build*" EXCLUDE
    PATTERN "*/tests*" EXCLUDE
    PATTERN "*/benchmarks*" EXCLUDE
)

foreach(file  ${SOURCE_FILES})
    get_filename_component(_fileName "${file}" NAME)
    string(REPLACE "${SRC}/" "" _fileDir "${file}")
    string(REPLACE "${_fileName}" "" _fileDir "${_fileDir}")
    install(FILES "${file}" DESTINATION "src/ossia/${_fileDir}")
endforeach()

file(GENERATE OUTPUT library.json CONTENT [=[{
  "name": "libossia",
  "version": "1.0.0",
  "keywords": "sound, media, show control",
  "description": "libossia",
  "authors": [ ],
  "repository": {
    "type": "git",
    "url": "https://github.com/ossia/libossia.git"
  },
  "frameworks": "arduino",
  "platforms": "*"
}
]=]
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ossia-config.hpp" DESTINATION "include/")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/library.json" DESTINATION ".")
