cmake_minimum_required(VERSION 3.0)

project(libossia CXX)

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

set(OSSIA_PROTOCOL_MIDI TRUE)
set(OSSIA_PROTOCOL_HTTP TRUE)

if(IOS OR CMAKE_SYSTEM_NAME MATCHES Emscripten)
  set(OSSIA_PROTOCOL_MIDI FALSE)
  set(OSSIA_PROTOCOL_HTTP FALSE)
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
  if(${Qt5_FOUND})
    set(SRCS ${SRCS} ${HTTP_HEADERS} ${HTTP_SRCS})
    set(CMAKE_AUTOMOC 1)
  else()
    set(OSSIA_PROTOCOL_HTTP FALSE)
  endif()
endif()

if(APPLE AND OSSIA_FRAMEWORK)
  add_library(ossia MODULE ${SRCS} ${C_API_SRCS})
  set_target_properties(ossia PROPERTIES BUNDLE TRUE)
else()
  if(OSSIA_STATIC)
    add_library(ossia STATIC ${SRCS} ${C_API_SRCS})
  else()
    add_library(ossia SHARED ${SRCS} ${C_API_SRCS})
  endif()
endif()

target_link_libraries(ossia PUBLIC boost)

if(OSSIA_PROTOCOL_MIDI)
  target_link_libraries(ossia PRIVATE ModernMIDI)
endif()

if(OSSIA_PROTOCOL_HTTP)
  target_link_libraries(ossia PUBLIC Qt5::Qml)
endif()

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

