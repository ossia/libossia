cmake_minimum_required(VERSION 3.0)

project(libossia CXX)

### Project options ###
set(ossia_VERSION 1.0.0)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)
include(cotire)

include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)
find_package(Boost REQUIRED)
find_package(KF5DNSSD QUIET NO_MODULE)
if(NOT KF5DNSSD)
  message(STATUS "Install KF5-DNSSD to build with Zeroconf / Bonjour support")
endif()

### Protocol setup ###
set(OSSIA_PROTOCOLS OSC Minuit)

option(OSSIA_PROTOCOL_MIDI "Enable MIDI protocol" ON)
option(OSSIA_PROTOCOL_HTTP "Enable HTTP protocol" ON)
option(OSSIA_PROTOCOL_WEBSOCKETS "Enable WebSockets protocol" OFF)
option(OSSIA_PROTOCOL_SERIAL "Enable Serial port protocol" OFF)
set(OSSIA_PROTOCOL_WEBSOCKETS ON)

if(IOS OR CMAKE_SYSTEM_NAME MATCHES Emscripten)
  set(OSSIA_PROTOCOL_MIDI FALSE)
  set(OSSIA_PROTOCOL_HTTP FALSE)
  set(OSSIA_PROTOCOL_WEBSOCKETS FALSE)
  set(OSSIA_PROTOCOL_SERIAL FALSE)
endif()

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@executable_path;")
endif()

if(TARGET ModernMIDI)
  set(OSSIA_PROTOCOL_MIDI ON)
else()
  set(OSSIA_PROTOCOL_MIDI OFF)
endif()

include(ossia_sources.cmake)

if(OSSIA_PROTOCOL_HTTP)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Qml)
  if(${Qt5Qml_FOUND})
    set(CMAKE_AUTOMOC 1)
  else()
    set(OSSIA_PROTOCOL_HTTP FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Qml WebSockets)
  if(${Qt5WebSockets_FOUND})
    set(CMAKE_AUTOMOC 1)
  else()
    set(OSSIA_PROTOCOL_WEBSOCKETS FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_SERIAL)
  find_package(Qt5 5.6 QUIET COMPONENTS Core SerialPort)
  if(${Qt5SerialPort_FOUND})
    set(CMAKE_AUTOMOC 1)
  else()
    set(OSSIA_PROTOCOL_SERIAL FALSE)
  endif()
endif()

### Library ###
if(APPLE AND OSSIA_FRAMEWORK)
  add_library(ossia MODULE ${SRCS} ${C_API_SRCS})
  set_target_properties(ossia PROPERTIES BUNDLE TRUE)
else()
  if(OSSIA_STATIC OR NOT BUILD_SHARED_LIBS)
    add_library(ossia STATIC ${SRCS} ${C_API_SRCS})
  else()
    add_library(ossia SHARED ${SRCS} ${C_API_SRCS})
  endif()
endif()

target_link_libraries(ossia PUBLIC boost)

# Additional protocols
if(OSSIA_PROTOCOL_MIDI)
  target_sources(ossia PRIVATE ${MIDI_SRCS} ${MIDI_HEADERS})
  target_link_libraries(ossia PRIVATE ModernMIDI)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} MIDI)
endif()

if(OSSIA_PROTOCOL_HTTP)
  target_sources(ossia PRIVATE ${HTTP_HEADERS} ${HTTP_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::Qml)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} HTTP)
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  target_sources(ossia PRIVATE ${WS_CLIENT_HEADERS} ${WS_CLIENT_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::Qml Qt5::WebSockets)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} WebSockets)
endif()

if(OSSIA_PROTOCOL_SERIAL)
  target_sources(ossia PRIVATE ${SERIAL_HEADERS} ${SERIAL_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::SerialPort)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Serial)
endif()

# Additional features
if(OSSIA_JAVA)
    target_sources(ossia PRIVATE ${JAVA_HEADERS} ${JAVA_SRCS})
    target_include_directories(ossia PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-java"
        "${OSSIA_3RDPARTY_FOLDER}/jni_hpp/include"
        "${OSSIA_3RDPARTY_FOLDER}/jni_hpp/test"
        )
endif()

if(KF5DNSSD_FOUND)
    set(OSSIA_ZEROCONF 1)
    target_link_libraries(ossia PRIVATE KF5::DNSSD)
    target_compile_definitions(ossia PRIVATE OSSIA_ZEROCONF)
endif()

# Useful properties
set_target_properties(ossia PROPERTIES OSSIA_PROTOCOLS "${OSSIA_PROTOCOLS}")
source_group(Editor REGULAR_EXPRESSION "ossia/editor.*")
source_group(Network REGULAR_EXPRESSION "ossia/network.*")

# Compile options
target_compile_definitions(ossia PRIVATE RAPIDJSON_HAS_STDSTRING=1)

if(WIN32)
    target_compile_definitions(ossia PUBLIC
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN)
    if(OSSIA_LTO)
      target_compile_options(ossia PUBLIC "$<$<CONFIG:Release>:/GL>")
    endif()
    target_link_libraries(ossia PRIVATE Ws2_32 winmm)
endif()

find_package(Threads)
target_link_libraries(ossia PUBLIC ${CMAKE_THREAD_LIBS_INIT} )

target_compile_options(ossia PRIVATE ${OSSIA_COMPILE_OPTIONS})
target_link_libraries(ossia PRIVATE ${OSSIA_LINK_OPTIONS})

if(OSSIA_SANITIZE)
    sanitize_build(ossia)
endif()
use_gold(ossia)
add_linker_warnings(ossia)

if(BUILD_SHARED_LIBS AND NOT "${OSSIA_STATIC}")
  set_target_properties(ossia PROPERTIES
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1
      VERSION ${ossia_VERSION}
      SOVERSION 1)
  if(${OSSIA_NO_SONAME})
      set_target_properties(ossia PROPERTIES NO_SONAME 1)
  endif()

else()
  set_target_properties(ossia PROPERTIES
      VERSION ${ossia_VERSION})
endif()

generate_export_header(ossia BASE_NAME OSSIA)

target_include_directories(ossia
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

# Note : should be SYSTEM PUBLIC but due to sakra/cotire:105 we cannot.
target_include_directories(ossia
  PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/variant/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/spdlog/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/brigand>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/fmt>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/chobo-shl/include>
)

target_include_directories(ossia SYSTEM
  PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/rapidjson/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/ModernMIDI>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/ModernMIDI/third_party>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/oscpack>

      $<INSTALL_INTERFACE:include/nano-signal-slot>
      $<INSTALL_INTERFACE:include>
  )

if(NOT OSSIA_NO_INSTALL)
    include(ossia_install.cmake)
endif()
include(ossia_tooling.cmake)


### Some status info ###
message(STATUS "OSSIA - Sanitize: ${OSSIA_SANITIZE}")
message(STATUS "OSSIA - Tidy: ${OSSIA_TIDY}")
message(STATUS "OSSIA - Static: ${OSSIA_STATIC}")
message(STATUS "OSSIA - Coverage: ${OSSIA_COVERAGE}")
message(STATUS "OSSIA - Examples: ${OSSIA_EXAMPLES}")
message(STATUS "OSSIA - Tests: ${OSSIA_TESTING}")
message(STATUS "OSSIA - CI: ${OSSIA_CI}")
message(STATUS "OSSIA - Framework: ${OSSIA_FRAMEWORK}")
message(STATUS "OSSIA - Java: ${OSSIA_JAVA}")
message(STATUS "OSSIA - Protocols: ${OSSIA_PROTOCOLS}")
message(STATUS "OSSIA - Zeroconf: ${KF5DNSSD_FOUND}")
message(STATUS "OSSIA - LTO: ${OSSIA_LTO}")
message(STATUS "OSSIA - PureData: ${OSSIA_PD}")
message(STATUS "OSSIA - OSX Architectures: ${OSSIA_OSX_FAT_LIBRARIES}")
message(STATUS "OSSIA - OSX Retrocompatibility: ${OSSIA_OSX_RETROCOMPATIBILITY}")

set_property(TARGET ossia PROPERTY COTIRE_CXX_PREFIX_HEADER_INIT "${CMAKE_CURRENT_SOURCE_DIR}/ossia/prefix.hpp")
if(NOT "${OSSIA_SANITIZE}" AND NOT "${OSSIA_DISABLE_COTIRE}" AND NOT "${DISABLE_COTIRE}")
cotire(ossia)
endif()

