cmake_minimum_required(VERSION 3.0)

project(OSSIA_common CXX)

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

set(SRCS
    "${OSSIA_HEADERS_FOLDER}/Editor/Automation.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Clock.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveAbstract.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Curve.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveSegment/Easing.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveSegment/Empty.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveSegment.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveSegment/Linear.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveSegment/MathUtil.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveSegment/Power.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/CurveSegment/Sin.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Domain.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/ExpressionAtom.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/ExpressionComposition.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Expression.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/ExpressionNot.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/ExpressionPulse.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Loop.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Mapper.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Message.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Scenario.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/StateElement.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/State.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/TimeConstraint.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/TimeEvent.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/TimeNode.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/TimeProcess.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/TimeValue.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Behavior.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Bool.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Char.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Destination.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Float.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Impulse.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Int.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/String.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Tuple.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/ValueBase.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/ValueComparisonImpl.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Value.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/ValueTraits.h"
    "${OSSIA_HEADERS_FOLDER}/Editor/Value/Vec.h"
    "${OSSIA_HEADERS_FOLDER}/Misc/CallbackContainer.h"
    "${OSSIA_HEADERS_FOLDER}/Misc/Container.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Address.h"
    "${OSSIA_HEADERS_FOLDER}/Network/AddressProperties.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Device.h"
    "${OSSIA_HEADERS_FOLDER}/Network/NetworkLogger.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Node.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Protocol.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Protocol/Local.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Protocol/MIDI.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Protocol/Minuit.h"
    "${OSSIA_HEADERS_FOLDER}/Network/Protocol/OSC.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Automation_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Automation_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Clock_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Clock_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Curve_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Curve_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Domain_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Domain_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionAtom_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionAtom_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionComposition_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionComposition_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Expression_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Expression_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionNot_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionNot_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionPulse_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionPulse_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Loop_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Loop_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Mapper_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Mapper_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Message_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Scenario_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Scenario_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeConstraint_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeConstraint_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeEvent_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeEvent_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeNode_impl.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeNode_impl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeProcess_impl.h"


    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Automation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Clock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Curve.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Domain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionAtom.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionComposition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Expression.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionNot.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/ExpressionPulse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Loop.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Mapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Message.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Scenario.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/State.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeConstraint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeEvent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeProcess.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Value.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Address.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Protocol.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Protocol/MIDI_impl.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Protocol/MIDI_impl.cpp"
    )

add_library(OSSIA_common STATIC ${SRCS})
if(WIN32)
    target_compile_definitions(OSSIA_common PUBLIC
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS)
endif()

if(MSVC)
    target_compile_options(OSSIA_common PUBLIC
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        )
endif()

target_compile_options(OSSIA_common PUBLIC ${OSSIA_COMPILE_FLAGS})

set_target_properties(OSSIA_common PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 0)

generate_export_header(OSSIA_common BASE_NAME OSSIA)
target_include_directories(OSSIA_common PUBLIC
    "${OSSIA_HEADERS_FOLDER}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${OSSIA_3RDPARTY_FOLDER}/variant/include")


if(Boost_FOUND)
    # Allows the use of faster containers
    target_compile_definitions(OSSIA_common PUBLIC OSSIA_BOOST)
    target_include_directories(OSSIA_common PUBLIC ${Boost_INCLUDE_DIRS})
endif()
target_link_libraries(OSSIA_common PRIVATE ModernMIDI)


if(OSSIA_SANITIZE)
    sanitize_build(OSSIA_common)
    #debugmode_build(APIJamoma)
else()
    use_gold(OSSIA_common)
endif()
add_linker_warnings(OSSIA_common)


