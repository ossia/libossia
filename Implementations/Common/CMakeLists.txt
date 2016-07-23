cmake_minimum_required(VERSION 3.0)

project(OSSIA_common)
include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)

set(SRCS

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
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/StateElement.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeConstraint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeEvent.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeProcess.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/TimeValue.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Editor/Value.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/Misc/CallbackContainer.cpp"


    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Address.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/Protocol.cpp"
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

target_compile_options(OSSIA_common PRIVATE ${OSSIA_COMPILE_FLAGS})

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


if(OSSIA_SANITIZE)
    sanitize_build(OSSIA_common)
    #debugmode_build(APIJamoma)
else()
    use_gold(OSSIA_common)
endif()
add_linker_warnings(OSSIA_common)


