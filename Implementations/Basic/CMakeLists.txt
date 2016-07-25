cmake_minimum_required(VERSION 2.8)

project(BasicOSSIA)

include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)

find_package(Boost REQUIRED)

set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Address.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Address.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Device.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Node.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Node.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Protocol.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Protocol.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/BasicAddress.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicAddress.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicDevice.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicDevice.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicLocal.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicLocal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicMinuit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicMinuit.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicNode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicNode.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicOSC.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicOSC.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/BasicProtocol.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/Domain.hpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/OSC/Osc.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/OSC/MessageGenerator.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/OSC/Sender.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/OSC/Receiver.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/OSC/string_view.hpp"

    )

add_library(BasicOSSIA ${SRCS})
target_link_libraries(BasicOSSIA PRIVATE
    OSSIA_common
    oscpack)

target_include_directories(BasicOSSIA
    PRIVATE
      "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot"
    INTERFACE
      "${CMAKE_CURRENT_SOURCE_DIR}")

if(OSSIA_SANITIZE)
    sanitize_build(BasicOSSIA)
    #debugmode_build(APIJamoma)
else()
    use_gold(BasicOSSIA)
endif()
add_linker_warnings(BasicOSSIA)


