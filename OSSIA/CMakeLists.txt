cmake_minimum_required(VERSION 3.0)

project(OSSIA CXX)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)

include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)
find_package(Boost REQUIRED)
set(OSSIA_WITH_JAMOMA 1)
if(OSSIA_WITH_JAMOMA)

    find_package(Jamoma 0.6)
    if(NOT ${Jamoma_FOUND})
        message(FATAL_ERROR "Jamoma not found - indicate path to jamoma/share/cmake/Jamoma in CMAKE_PREFIX_PATH")
        return()
    endif()

    if(APPLE)
      set(CMAKE_INSTALL_NAME_DIR @rpath)
      set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
      set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@loader_path/../Frameworks/jamoma;@loader_path/../Frameworks/jamoma/extensions;@executable_path;/usr/local/jamoma/lib;")
    endif()

endif()
set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/algorithms.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/callback_container.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/destination_index.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/math.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/detail/ptr_container.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/automation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/automation.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/detail/Automation_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/automation/detail/Automation_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_abstract.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/easing.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/empty.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/linear.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/power.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/curve_segment/sin.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/detail/Curve_impl.tpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/curve/detail/Curve_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/editor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionAtom_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionAtom_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionComposition_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionComposition_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/Expression_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/Expression_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionNot_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionNot_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionPulse_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/detail/ExpressionPulse_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_atom.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_atom.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_composition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_composition.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_not.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_not.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_pulse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/expression/expression_pulse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/loop/detail/Loop_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/loop/detail/Loop_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/loop/loop.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/loop/loop.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/mapper/detail/Mapper_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/mapper/detail/Mapper_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/mapper/mapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/mapper/mapper.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/Clock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/Clock.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/Clock_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/Clock_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/Scenario_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/Scenario_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/TimeConstraint_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/TimeConstraint_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/TimeEvent_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/TimeEvent_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/TimeNode_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/TimeNode_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/detail/TimeProcess_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/scenario.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/scenario.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_constraint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_constraint.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_event.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_event.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_process.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_process.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/scenario/time_value.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/custom_state.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/detail/state_execution_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/detail/state_flatten_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/detail/state_print_visitor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/message.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/message.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state_element.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state_element.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/state/state.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/behavior.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/bool.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/char.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/destination.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/float.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/impulse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/int.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/string.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/tuple.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_base.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_comparison.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/value_traits.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/editor/value/vec.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/network.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/OSSIA.hpp"

)

set(NETWORK_V1_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Address.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Address.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/AddressProperties.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/Domain_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/Domain_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaAddress.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaAddress.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaDevice.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaDevice.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaNode.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/detail/JamomaProtocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Domain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/NetworkLogger.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaLocal.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaLocal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaMIDI.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaMIDI.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaMinuit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaMinuit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaOSC.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/detail/JamomaOSC.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/Local.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/MIDI.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/Minuit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v1/Protocol/OSC.hpp"
    )

set(NETWORK_V2_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/address.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/address.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/protocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/base/protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/domain.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/generic/generic_address.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/generic/generic_address.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/generic/generic_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/generic/generic_device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/generic/generic_node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/generic/generic_node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/local/local.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/local/local.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/minuit/minuit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/minuit/minuit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/osc/detail/message_generator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/osc/detail/osc.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/osc/detail/receiver.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/osc/detail/sender.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/osc/detail/string_view.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/osc/osc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ossia/network/v2/osc/osc.hpp"
)

add_library(OSSIA ${SRCS} ${NETWORK_V2_SRCS})
if(OSSIA_WITH_JAMOMA)
    target_sources(OSSIA PUBLIC ${NETWORK_V1_SRCS})
    target_link_libraries(OSSIA PUBLIC Jamoma::Foundation Jamoma::Modular)
endif()
target_link_libraries(OSSIA PUBLIC oscpack)

if(WIN32)
    target_compile_definitions(OSSIA PUBLIC
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS)

    if(OSSIA_WITH_JAMOMA)
        target_compile_definitions(OSSIA PUBLIC TT_PLATFORM_WIN)
    endif()
endif()

if(MSVC)
    target_compile_options(OSSIA PUBLIC
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        )
else()
    target_compile_options(OSSIA PUBLIC
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-unknown-pragmas
    -Wnon-virtual-dtor
    -pedantic
    -Woverloaded-virtual
    -pipe
    -Werror=return-type
    -Werror=trigraphs)
endif()

set_target_properties(OSSIA PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 0)

generate_export_header(OSSIA BASE_NAME OSSIA)
target_include_directories(OSSIA
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${OSSIA_3RDPARTY_FOLDER}/variant/include"
        "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot")


target_include_directories(OSSIA PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(OSSIA PRIVATE ModernMIDI)


if(OSSIA_SANITIZE)
    sanitize_build(OSSIA)
    #debugmode_build(APIJamoma)
else()
    use_gold(OSSIA)
endif()
add_linker_warnings(OSSIA)
