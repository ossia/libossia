cmake_minimum_required(VERSION 3.0)

project(libossia CXX)

### Project options ###
set(ossia_VERSION 1.0.0)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)

include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)
find_package(Boost REQUIRED)

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

include(ossia_sources.cmake)

if(OSSIA_PROTOCOL_MIDI)
    set(API_HEADERS ${API_HEADERS} ${MIDI_HEADERS})
    set(API_HEADERS ${API_HEADERS} ${MIDI_HEADERS})
    set(SRCS ${SRCS} ${MIDI_SRCS} ${API_HEADERS})
endif()

if(OSSIA_PROTOCOL_HTTP)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Qml)
  if(${Qt5Qml_FOUND})
    set(SRCS ${SRCS} ${HTTP_HEADERS} ${HTTP_SRCS})
    set(CMAKE_AUTOMOC 1)
  else()
    set(OSSIA_PROTOCOL_HTTP FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Qml WebSockets)
  if(${Qt5WebSockets_FOUND})
    set(SRCS ${SRCS} ${WS_CLIENT_HEADERS} ${WS_CLIENT_SRCS})
    set(CMAKE_AUTOMOC 1)
  else()
    set(OSSIA_PROTOCOL_WEBSOCKETS FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_SERIAL)
  find_package(Qt5 5.6 QUIET COMPONENTS Core SerialPort)
  if(${Qt5SerialPort_FOUND})
    set(SRCS ${SRCS} ${SERIAL_HEADERS} ${SERIAL_SRCS})
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

if(OSSIA_PROTOCOL_MIDI)
  target_link_libraries(ossia PRIVATE ModernMIDI)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} MIDI)
endif()

if(OSSIA_PROTOCOL_HTTP)
  target_link_libraries(ossia PUBLIC Qt5::Qml)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} HTTP)
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  target_link_libraries(ossia PUBLIC Qt5::Qml Qt5::WebSockets)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} WebSockets)
endif()

if(OSSIA_PROTOCOL_SERIAL)
  target_link_libraries(ossia PUBLIC Qt5::SerialPort)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Serial)
endif()

set_target_properties(ossia PROPERTIES OSSIA_PROTOCOLS "${OSSIA_PROTOCOLS}")
source_group(Editor REGULAR_EXPRESSION "ossia/editor.*")
source_group(Network REGULAR_EXPRESSION "ossia/network.*")

# Compile options
if(WIN32)
    target_compile_definitions(ossia PUBLIC
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN)
    target_compile_options(ossia PUBLIC "/bigobj")
    target_link_libraries(ossia PRIVATE Ws2_32 winmm)
endif()

if(MSVC)
    target_compile_options(ossia PUBLIC
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        "/wd4251" # DLL stuff
        "/wd4275" # DLL stuff
        "/wd4244" # return : conversion from foo to bar, possible loss of data
        "/wd4800" # conversion from int to bool, performance warning
        )
else()
    target_compile_options(ossia PUBLIC
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-unknown-pragmas
    -Wnon-virtual-dtor
    -pedantic
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -pipe
    -Werror=return-type
    -Werror=trigraphs)

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(ossia PUBLIC -Wmisleading-indentation)
    endif()
endif()

if(OSSIA_SANITIZE)
    sanitize_build(ossia)
else()
    use_gold(ossia)
endif()
add_linker_warnings(ossia)


if(BUILD_SHARED_LIBS AND NOT OSSIA_STATIC)
  set_target_properties(ossia PROPERTIES
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1
      VERSION ${ossia_VERSION}
      SOVERSION 1)
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

target_include_directories(ossia SYSTEM
  PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/variant/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/rapidjson/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/ModernMIDI>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/ModernMIDI/third_party>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/oscpack>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/spdlog/include>

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
message(STATUS "OSSIA - Examples: ${OSSIA_EXAMPLES}")
message(STATUS "OSSIA - Tests: ${OSSIA_TESTING}")
message(STATUS "OSSIA - Static: ${OSSIA_STATIC}")
message(STATUS "OSSIA - Framework: ${OSSIA_FRAMEWORK}")
message(STATUS "OSSIA - Protocols: ${OSSIA_PROTOCOLS}")
include(cotire)
cotire(ossia)
