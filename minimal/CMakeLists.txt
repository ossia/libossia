cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(ossia C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(OSSIA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(OSSIA_3RDPARTY_FOLDER ${OSSIA_ROOT}/3rdparty)
set(SRC ${OSSIA_ROOT}/src/ossia)
include(${OSSIA_ROOT}/src/ossia_sources.cmake)

configure_file(${OSSIA_ROOT}/src/ossia-config.hpp.in ossia-config.hpp)

add_library(ossia)
target_compile_options(ossia PRIVATE -std=c++20)
target_compile_definitions(ossia PRIVATE OSSIA_EXPORTS=1 OSSIA_FREESTANDING=1)
target_link_libraries(ossia PRIVATE -Wl,-z,defs -fuse-ld=mold)
target_include_directories(ossia PRIVATE /home/jcelerier/ossia/score/3rdparty/libossia/3rdparty/boost_1_82_0)

target_include_directories(ossia PRIVATE
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

target_sources(ossia PRIVATE

"${SRC}/detail/lockfree_queue.hpp"
"${SRC}/context.cpp"
"${SRC}/detail/thread.cpp"
"${SRC}/detail/any_map.cpp"

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
"${SRC}/dataflow/execution_state.cpp"
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
