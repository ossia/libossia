cmake_minimum_required(VERSION 3.0)

project(libossia CXX)

### Project options ###
set(OSSIA_PROTOCOLS OSC Minuit)

set(ossia_VERSION 1.0.0)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)
include(cotire)

include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
find_package(Boost REQUIRED)

if(OSSIA_QT)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Gui Qml Quick)
  if(NOT "${Qt5Quick_FOUND}")
    set(OSSIA_QT OFF)
  else()
    set(CMAKE_AUTOMOC 1)
  endif()
endif()

### Protocol setup ###
# These two are always here:
set(OSSIA_PROTOCOL_OSC 1)
set(OSSIA_PROTOCOL_MINUIT 1)

if(IOS OR CMAKE_SYSTEM_NAME MATCHES Emscripten)
  set(OSSIA_PROTOCOL_MIDI FALSE)
  set(OSSIA_PROTOCOL_HTTP FALSE)
  set(OSSIA_PROTOCOL_WEBSOCKETS FALSE)
  set(OSSIA_PROTOCOL_SERIAL FALSE)
endif()

if(NOT OSSIA_QT)
  set(OSSIA_PROTOCOL_HTTP FALSE)
  set(OSSIA_PROTOCOL_WEBSOCKETS FALSE)
  set(OSSIA_PROTOCOL_SERIAL FALSE)
endif()

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@executable_path;")
endif()

if(NOT TARGET ModernMIDI)
  set(OSSIA_PROTOCOL_MIDI OFF)
  set(OSSIA_DATAFLOW OFF)
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

if(OSSIA_PROTOCOL_PHIDGETS)
  find_package(Phidgets)
  find_package(USB)
  find_package(DL)

  if(NOT "${LIBPHIDGETS_FOUND}")
    set(OSSIA_PROTOCOL_PHIDGETS FALSE)
  endif()
  if(NOT "${LIBUSB_FOUND}")
    set(OSSIA_PROTOCOL_PHIDGETS FALSE)
  endif()
  if(NOT "${Libdl_FOUND}")
    set(OSSIA_PROTOCOL_PHIDGETS FALSE)
  endif()
endif()

if (NOT OSSIA_CI)
  include(ossia_git_info)
  get_commit_sha(GIT_SHA)
  get_commit_date(GIT_DATE)
else()
  if ( $ENV{TRAVIS} MATCHES True )
    set(GIT_SHA $ENV{TRAVIS_COMMIT})
    string(SUBSTRING ${GIT_SHA} 0 7 GIT_SHA)
  elseif ( $ENV{APPVEYOR} MATCHES True )
    set(GIT_SHA $ENV{APPVEYOR_REPO_COMMIT})
    string(SUBSTRING ${GIT_SHA} 0 7 GIT_SHA)
    set(GIT_DATE $ENV{APPVEYOR_REPO_COMMIT_TIMESTAMP})
  endif()
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_info.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/git_info.cpp" @ONLY)
list(APPEND SRCS "${CMAKE_CURRENT_BINARY_DIR}/git_info.cpp" git_info.h)

### Library ###
if(APPLE AND OSSIA_FRAMEWORK)
  add_library(ossia MODULE ${SRCS})
  set_target_properties(ossia PROPERTIES BUNDLE TRUE)
else()
  if(OSSIA_STATIC OR NOT BUILD_SHARED_LIBS)
    add_library(ossia STATIC ${SRCS})
  else()
    add_library(ossia SHARED ${SRCS})
  endif()
endif()

target_compile_definitions(ossia PUBLIC TINYSPLINE_DOUBLE_PRECISION)

if(NOT ANDROID)
  target_link_libraries(ossia PUBLIC boost)
endif()
# Additional protocols
if(OSSIA_PROTOCOL_MIDI)
  target_sources(ossia PRIVATE ${OSSIA_MIDI_SRCS} ${OSSIA_MIDI_HEADERS})
  target_link_libraries(ossia PRIVATE ModernMIDI)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} MIDI)
endif()

if(OSSIA_PROTOCOL_OSCQUERY)
  target_sources(ossia PRIVATE ${OSSIA_OSCQUERY_SRCS} ${OSSIA_OSCQUERY_HEADERS})
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} OSCQuery)
  target_compile_definitions(ossia PUBLIC ASIO_STANDALONE=1)
  target_include_directories(ossia SYSTEM PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/asio/asio/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/websocketpp>
      )
endif()

if(OSSIA_PROTOCOL_HTTP)
  target_sources(ossia PRIVATE ${OSSIA_HTTP_HEADERS} ${OSSIA_HTTP_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::Qml)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} HTTP)
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  target_sources(ossia PRIVATE ${OSSIA_WS_CLIENT_HEADERS} ${OSSIA_WS_CLIENT_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::Qml Qt5::WebSockets)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} WebSockets)
endif()

if(OSSIA_PROTOCOL_SERIAL)
  target_sources(ossia PRIVATE ${OSSIA_SERIAL_HEADERS} ${OSSIA_SERIAL_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::SerialPort)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Serial)
endif()

if(OSSIA_PROTOCOL_PHIDGETS)
  target_sources(ossia PRIVATE ${OSSIA_PHIDGETS_HEADERS} ${OSSIA_PHIDGETS_SRCS})
  target_include_directories(ossia PUBLIC ${LIBPHIDGETS_INCLUDE_DIR})
  target_link_libraries(ossia PUBLIC ${LIBPHIDGETS_LIBRARIES} ${LIBUSB_LIBRARIES} ${LIBDL_LIBRARIES})
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Phidgets)
endif()

# Additional features
if(OSSIA_C)
  target_sources(ossia PRIVATE ${OSSIA_C_HEADERS} ${OSSIA_C_SRCS})
  target_include_directories(ossia PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/ossia-c"
  )
endif()

if(OSSIA_CPP)
    target_sources(ossia PRIVATE ${OSSIA_CPP_HEADERS} ${OSSIA_CPP_SRCS})
    target_include_directories(ossia PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/ossia-cpp"
        )
endif()

if(OSSIA_JAVA)
  target_sources(ossia PRIVATE ${OSSIA_JAVA_HEADERS} ${OSSIA_JAVA_SRCS})
  target_include_directories(ossia PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/ossia-java"
  )
  target_include_directories(ossia SYSTEM PRIVATE
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/jni_hpp/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/jni_hpp/test>
  )
endif()

if(OSSIA_QT)
  target_sources(ossia PRIVATE ${OSSIA_QT_HEADERS} ${OSSIA_QT_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick)
  target_include_directories(ossia PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt"
   )
 add_custom_target(ossia-qml-sources SOURCES ${OSSIA_QML_SRCS})
 if(OSSIA_DISABLE_QT_PLUGIN)
   target_compile_definitions(ossia PRIVATE OSSIA_DISABLE_QT_PLUGIN)
 endif()
endif()

if(OSSIA_DNSSD)
  target_sources(ossia PRIVATE $<TARGET_OBJECTS:Servus>)
  get_target_property(SERVUS_LINK_LIBRARIES Servus INTERFACE_LINK_LIBRARIES)
  get_target_property(SERVUS_COMPILE_DEFINITIONS Servus INTERFACE_COMPILE_DEFINITIONS)
  get_target_property(SERVUS_INCLUDE_DIRS Servus INTERFACE_INCLUDE_DIRECTORIES)
  target_link_libraries(ossia PRIVATE ${SERVUS_LINK_LIBRARIES})
  target_include_directories(ossia PRIVATE ${SERVUS_INCLUDE_DIRS})
  target_compile_definitions(ossia PRIVATE ${SERVUS_COMPILE_DEFINITIONS})
endif()

if(OSSIA_DATAFLOW)
  target_sources(ossia PRIVATE ${OSSIA_DATAFLOW_HEADERS} ${OSSIA_DATAFLOW_SRCS})
endif()
# Useful properties
set_target_properties(ossia PROPERTIES OSSIA_PROTOCOLS "${OSSIA_PROTOCOLS}")
source_group(Editor REGULAR_EXPRESSION "ossia/editor.*")
source_group(Network REGULAR_EXPRESSION "ossia/network.*")

# Compile options
target_compile_definitions(ossia PUBLIC
  RAPIDJSON_HAS_STDSTRING=1
  $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING>
  $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_SAFE_MODE>
  )

if(WIN32)
    target_compile_definitions(ossia PUBLIC
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN)
    if(OSSIA_LTO)
      target_compile_options(ossia PUBLIC "$<$<CONFIG:Release>:/GL>")
    endif()
    target_link_libraries(ossia PRIVATE Ws2_32 winmm)
    if(MINGW)
        target_link_libraries(ossia PRIVATE Mswsock)
    endif()
endif()

find_package(Threads)
target_link_libraries(ossia PUBLIC ${CMAKE_THREAD_LIBS_INIT} )

target_compile_options(ossia PRIVATE ${OSSIA_COMPILE_OPTIONS})
target_link_libraries(ossia PRIVATE ${OSSIA_LINK_OPTIONS})

if(OSSIA_SANITIZE)
    sanitize_build(ossia)
else()
    # TODO fix the weak symbol problem on OS X release build with fsanitize=address
    add_linker_warnings(ossia)
endif()
use_gold(ossia)

if(OSSIA_LTO)
  enable_lto(ossia)
endif()

if(BUILD_SHARED_LIBS AND NOT "${OSSIA_STATIC}")
  set_target_properties(ossia PROPERTIES
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1
      )
  if(${OSSIA_NO_SONAME})
      #set_target_properties(ossia PROPERTIES NO_SONAME 1)
  else()
    set_target_properties(ossia PROPERTIES
      VERSION ${ossia_VERSION}
      SOVERSION 1
      )
  endif()

else()
  set_target_properties(ossia PROPERTIES
      VERSION ${ossia_VERSION}
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1)
endif()

generate_export_header(ossia BASE_NAME OSSIA)
configure_file(ossia-config.hpp.in ossia-config.hpp)

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
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/brigand/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/fmt>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/src>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/chobo-shl/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/GSL/include>
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
message(STATUS "OSSIA - Build Type: ${CMAKE_BUILD_TYPE}")
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
message(STATUS "OSSIA - OSX Architectures: ${OSSIA_OSX_FAT_LIBRARIES}")
message(STATUS "OSSIA - OSX Retrocompatibility: ${OSSIA_OSX_RETROCOMPATIBILITY}")
message("")
message(STATUS "OSSIA - bindings ----------")
message(STATUS "OSSIA - PureData: ${OSSIA_PD}")
message(STATUS "OSSIA - Max: ${OSSIA_MAX}")
message(STATUS "OSSIA - Python: ${OSSIA_PYTHON}")
message(STATUS "OSSIA - Unity3d: ${OSSIA_UNITY3D}")
message(STATUS "OSSIA - Java: ${OSSIA_JAVA}")
message(STATUS "OSSIA - Qt: ${OSSIA_QT}")
message(STATUS "OSSIA - C: ${OSSIA_C}")
message(STATUS "OSSIA - CPP: ${OSSIA_CPP}")
set_property(TARGET ossia PROPERTY COTIRE_CXX_PREFIX_HEADER_INIT "${CMAKE_CURRENT_SOURCE_DIR}/ossia/prefix.hpp")
if(NOT "${OSSIA_SANITIZE}" AND NOT "${OSSIA_DISABLE_COTIRE}" AND NOT "${DISABLE_COTIRE}")
cotire(ossia)
endif()

