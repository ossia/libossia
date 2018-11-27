cmake_minimum_required(VERSION 3.9)

project(libossia CXX)

### Project options ###
set(OSSIA_PROTOCOLS)

set(CMAKE_AUTOMOC 0)
set(ossia_VERSION 1.0.0)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)
include(cotire)

include(GenerateExportHeader)
set(WINDOWS_EXPORT_ALL_SYMBOLS FALSE)

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@executable_path;")
endif()

find_package(Boost REQUIRED)

if(OSSIA_QT)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Gui Qml Quick)
  if(NOT "${Qt5Core_FOUND}")
    set(OSSIA_QT OFF)
  endif()
  if(NOT "${Qt5Quick_FOUND}")
    set(OSSIA_QT OFF)
  endif()
  if(NOT DEFINED OSSIA_QML_DEVICE)
    set(OSSIA_QML_DEVICE 1)
  endif()
endif()


### Library ###
include(ossia_sources.cmake)

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

if (OSSIA_CI)
  if ( $ENV{TRAVIS} MATCHES true )
    message(STATUS "We're building on Travis, SHA: $ENV{TRAVIS_COMMIT}, TAG: $ENV{TRAVIS_TAG}")
    set(GIT_SHA $ENV{TRAVIS_COMMIT})
    string(SUBSTRING ${GIT_SHA} 0 7 GIT_SHA)
    set(GIT_SHA "$ENV{TRAVIS_TAG}-${GIT_SHA}")
  elseif ( $ENV{APPVEYOR} MATCHES True )
    set(GIT_SHA $ENV{APPVEYOR_REPO_COMMIT})
    string(SUBSTRING ${GIT_SHA} 0 7 GIT_SHA)
    set(GIT_DATE $ENV{APPVEYOR_REPO_COMMIT_TIMESTAMP})
    set(GIT_SHA "$ENV{APPVEYOR_REPO_TAG_NAME}-${GIT_SHA}")
    message(STATUS "We're building on Appveyor, SHA: $ENV{APPVEYOR_REPO_COMMIT}, last commit on $ENV{APPVEYOR_REPO_COMMIT_TIMESTAMP}")
  endif()
else()
  include(ossia_git_info)
  get_commit_sha(GIT_SHA)
  get_commit_date(GIT_DATE)
endif()

message(STATUS "build against git revision : ${GIT_SHA}, last commit on ${GIT_DATE}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_info.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/git_info.cpp" @ONLY)

target_sources(ossia PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/git_info.cpp" git_info.h)

if(NOT ANDROID)
  target_link_libraries(ossia PUBLIC Boost::boost)
endif()

### Protocol setup ###
# These two are always here:
if(IOS OR CMAKE_SYSTEM_NAME MATCHES Emscripten)
  set(OSSIA_PROTOCOL_AUDIO FALSE)
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

if(NOT TARGET RtMidi17)
  set(OSSIA_PROTOCOL_MIDI OFF)
endif()

if(NOT OSSIA_PROTOCOL_MIDI OR NOT OSSIA_PROTOCOL_AUDIO)
  set(OSSIA_DATAFLOW OFF)
endif()

if(OSSIA_PROTOCOL_AUDIO)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} AUDIO)
endif()

if(NOT OSSIA_DATAFLOW)
  set(OSSIA_EDITOR OFF)
endif()

function(ossia_link_jack)
  find_path(Jack_INCLUDE_DIR
    NAMES jack/jack.h
    HINTS
      ${CMAKE_CURRENT_SOURCE_DIR}
      /opt/cross-pi-gcc/usr/include/
      "${OSSIA_SDK}/jack/include"
      )
  if(Jack_INCLUDE_DIR)
    target_include_directories(ossia PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/weakjack>)
    target_include_directories(ossia PUBLIC $<BUILD_INTERFACE:${Jack_INCLUDE_DIR}>)
    target_link_libraries(ossia PUBLIC ${CMAKE_DL_LIBS})
  endif()
endfunction()

if(OSSIA_PROTOCOL_MIDI)
  target_sources(ossia PRIVATE ${OSSIA_MIDI_SRCS} ${OSSIA_MIDI_HEADERS})
  target_link_libraries(ossia PRIVATE RtMidi17)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} MIDI)

  ossia_link_jack()
endif()

if(OSSIA_PROTOCOL_OSC)
  target_sources(ossia PRIVATE ${OSSIA_OSC_SRCS} ${OSSIA_OSC_HEADERS})
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} OSC)
endif()

if(OSSIA_PROTOCOL_MINUIT)
  target_sources(ossia PRIVATE ${OSSIA_MINUIT_SRCS} ${OSSIA_MINUIT_HEADERS})
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Minuit)
endif()

if(OSSIA_PROTOCOL_OSCQUERY)
  target_sources(ossia PRIVATE ${OSSIA_OSCQUERY_SRCS} ${OSSIA_OSCQUERY_HEADERS})
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} OSCQuery)
  target_compile_definitions(ossia PUBLIC ASIO_STANDALONE=1)
endif()

if(OSSIA_PROTOCOL_HTTP)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Qml)
  if(${Qt5Qml_FOUND})
    target_sources(ossia PRIVATE ${OSSIA_HTTP_HEADERS} ${OSSIA_HTTP_SRCS})
    target_link_libraries(ossia PUBLIC Qt5::Qml)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} HTTP)
  else()
    set(OSSIA_PROTOCOL_HTTP FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Qml WebSockets)
  if(${Qt5WebSockets_FOUND})
    target_sources(ossia PRIVATE ${OSSIA_WS_CLIENT_HEADERS} ${OSSIA_WS_CLIENT_SRCS})
    target_link_libraries(ossia PUBLIC Qt5::Qml Qt5::WebSockets)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} WebSockets)
  else()
    set(OSSIA_PROTOCOL_WEBSOCKETS FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_SERIAL)
  find_package(Qt5 5.6 QUIET COMPONENTS Core SerialPort)
  if(${Qt5SerialPort_FOUND})
    target_sources(ossia PRIVATE ${OSSIA_SERIAL_HEADERS} ${OSSIA_SERIAL_SRCS})
    target_link_libraries(ossia PUBLIC Qt5::SerialPort)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Serial)
  else()
    set(OSSIA_PROTOCOL_SERIAL FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_PHIDGETS)
  find_package(Phidgets)

  if(NOT "${LIBPHIDGETS_FOUND}")
    set(OSSIA_PROTOCOL_PHIDGETS FALSE)
  endif()

  if(UNIX AND NOT APPLE)
    find_package(USB)
    if(NOT "${LIBUSB_FOUND}")
      set(OSSIA_PROTOCOL_PHIDGETS FALSE)
    endif()
  endif()

  if(OSSIA_PROTOCOL_PHIDGETS)
    target_sources(ossia PRIVATE ${OSSIA_PHIDGETS_HEADERS} ${OSSIA_PHIDGETS_SRCS})
    target_include_directories(ossia PUBLIC ${LIBPHIDGETS_INCLUDE_DIR})
    target_link_libraries(ossia PUBLIC ${LIBPHIDGETS_LIBRARIES} ${CMAKE_DL_LIBS})
    if(UNIX AND NOT APPLE)
        target_link_libraries(ossia PUBLIC ${LIBUSB_LIBRARIES})
    elseif(APPLE)
        target_link_libraries(ossia PUBLIC "-framework Phidget22")
    endif()
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Phidgets)
  endif()
endif()

if(OSSIA_PROTOCOL_LEAPMOTION)
  find_package(LeapMotion)
  if(LeapMotion_FOUND)
    target_sources(ossia PRIVATE ${OSSIA_LEAPMOTION_HEADERS} ${OSSIA_LEAPMOTION_SRCS})
    target_link_libraries(ossia PUBLIC LeapMotion)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} LeapMotion)
  else()
    set(OSSIA_PROTOCOL_LEAPMOTION FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_JOYSTICK)
  find_package(SDL2)
  if(TARGET SDL2::SDL2-static)
    target_sources(ossia PRIVATE ${OSSIA_JOYSTICK_SRCS} ${OSSIA_JOYSTICK_HEADERS})
    target_link_libraries(ossia PRIVATE SDL2::SDL2-static)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Joystick)
  elseif(TARGET SDL2::SDL2)
    target_sources(ossia PRIVATE ${OSSIA_JOYSTICK_SRCS} ${OSSIA_JOYSTICK_HEADERS})
    target_link_libraries(ossia PRIVATE SDL2::SDL2)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Joystick)
  else()
    set(OSSIA_PROTOCOL_JOYSTICK FALSE)
  endif()
endif()

if(OSSIA_PROTOCOL_WIIMOTE)
  target_sources(ossia PRIVATE ${OSSIA_WIIMOTE_SRCS} ${OSSIA_WIIMOTE_HEADERS})
  target_include_directories(ossia PRIVATE "${WIIUSE_DIR}/src")
  target_link_libraries(ossia PRIVATE wiiuse)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Wiimote)
endif()

if (OSSIA_PROTOCOL_ARTNET)
  set(LIBARTNET_DIR "${OSSIA_3RDPARTY_FOLDER}/libartnet")
  add_subdirectory("${LIBARTNET_DIR}" libartnet)
  target_sources(ossia PRIVATE ${OSSIA_ARTNET_SRCS} ${OSSIA_ARTNET_HEADERS})
  target_link_libraries(ossia PRIVATE artnet)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} artnet)
endif()


# Additional features
if(OSSIA_C)
  target_sources(ossia PRIVATE ${OSSIA_C_HEADERS} ${OSSIA_C_SRCS})
  target_include_directories(ossia PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ossia-c>"
  )
endif()

if(OSSIA_CPP)
    target_sources(ossia PRIVATE ${OSSIA_CPP_HEADERS} ${OSSIA_CPP_SRCS})
    target_include_directories(ossia PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ossia-cpp>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        )
endif()

if(OSSIA_QT)
  qt5_wrap_cpp(cur_moc "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/qml_plugin.hpp" TARGET ossia)
  target_sources(ossia PRIVATE ${cur_moc})

  target_sources(ossia PRIVATE ${OSSIA_QT_HEADERS} ${OSSIA_QT_SRCS})
  target_link_libraries(ossia PUBLIC Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick)
  target_include_directories(ossia
    PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt>"
    PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/verdigris/src>
   )
  add_custom_target(ossia-qml-sources SOURCES ${OSSIA_QML_SRCS})
  if(OSSIA_DISABLE_QT_PLUGIN)
    target_compile_definitions(ossia PRIVATE OSSIA_DISABLE_QT_PLUGIN)
  endif()

  if(OSSIA_QML_DEVICE)
      target_sources(ossia PRIVATE ${OSSIA_QT_DEVICE_HEADERS} ${OSSIA_QT_DEVICE_SRCS})
  endif()

  if(OSSIA_QML_SCORE)
      target_sources(ossia PRIVATE ${OSSIA_QT_SCORE_HEADERS} ${OSSIA_QT_SCORE_SRCS})
  endif()
endif()

if(OSSIA_DNSSD)
  target_link_libraries(ossia PRIVATE Servus_iface)
endif()

if(OSSIA_DATAFLOW)
  set(OSSIA_PARALLEL 1)
  target_link_libraries(ossia PRIVATE tbb)

  target_sources(ossia PRIVATE ${OSSIA_DATAFLOW_HEADERS} ${OSSIA_DATAFLOW_SRCS})

  ossia_link_jack()
  if(TARGET portaudio)
    target_link_libraries(ossia PUBLIC portaudio)
  else()
    find_library(PORTAUDIO_LIBRARY
      NAMES
        libportaudio.a portaudio
      PATHS
        /usr/local/lib)
    if(PORTAUDIO_LIBRARY)
      target_link_libraries(ossia PUBLIC "${PORTAUDIO_LIBRARY}")
    endif()
  endif()

  set(SDL_BUILDING_LIBRARY TRUE)
  find_package(SDL)
  if(SDL_FOUND)
    target_include_directories(ossia PUBLIC ${SDL_INCLUDE_DIR})
    target_link_libraries(ossia PUBLIC ${SDL_LIBRARY})
  endif()

  if(APPLE)
      find_library(Foundation_FK Foundation)
      find_library(AVFoundation_FK AVFoundation)
      find_library(CoreMedia_FK CoreMedia)
      find_library(CoreFoundation_FK CoreFoundation)
      find_library(AudioToolbox_FK AudioToolbox)
      find_library(AudioUnit_FK AudioUnit)
      find_library(CoreServices_FK CoreServices)

      target_link_libraries(ossia PRIVATE
          ${Foundation_FK}
          ${AVFoundation_FK}
          ${CoreMedia_FK}
          ${CoreFoundation_FK}
          ${AudioToolbox_FK}
          ${AudioUnit_FK}
          ${CoreServices_FK}
          )
  endif()
endif()

if(OSSIA_EDITOR)
  target_sources(ossia PRIVATE ${OSSIA_EDITOR_HEADERS} ${OSSIA_EDITOR_SRCS})
endif()

# Useful properties
set_target_properties(ossia PROPERTIES OSSIA_PROTOCOLS "${OSSIA_PROTOCOLS}")
if (OSSIA_EDITOR)
  source_group(Editor REGULAR_EXPRESSION "ossia/editor.*")
endif()
source_group(Network REGULAR_EXPRESSION "ossia/network.*")

# Compile options
target_compile_definitions(ossia
  PUBLIC
    RAPIDJSON_HAS_STDSTRING=1
    TINYSPLINE_DOUBLE_PRECISION
    $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING>
    $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_SAFE_MODE>
  )

if(WIN32)
    target_compile_definitions(ossia PUBLIC
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN)
    if(OSSIA_LTO)
      if(MSVC)
        target_compile_options(ossia PUBLIC "$<$<CONFIG:Release>:/GL>")
      else()
        target_compile_options(ossia PUBLIC "$<$<CONFIG:Release>:-flto>")
      endif()
    endif()
    target_link_libraries(ossia PRIVATE ws2_32 winmm)
    if(MINGW)
        target_link_libraries(ossia PRIVATE mswsock)
    endif()

    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
      set_target_properties(ossia PROPERTIES OUTPUT_NAME "ossia$<$<CONFIG:Debug>:d>")
    else()
      set_target_properties(ossia PROPERTIES OUTPUT_NAME "ossia_x64$<$<CONFIG:Debug>:d>")
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
      C_VISIBILITY_PRESET hidden
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
      C_VISIBILITY_PRESET default
      CXX_VISIBILITY_PRESET default
      VISIBILITY_INLINES_HIDDEN 0)
endif()

if(OSSIA_HIDE_ALL_SYMBOLS)
set_target_properties(ossia PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
    )
endif()

generate_export_header(ossia BASE_NAME OSSIA)
configure_file(ossia-config.hpp.in ossia-config.hpp)

target_include_directories(ossia
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_include_directories(ossia SYSTEM
  PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/variant/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/spdlog/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/brigand/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/fmt/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/chobo-shl/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/frozen/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/bitset2>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/GSL/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/flat_hash_map>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/flat/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/concurrentqueue>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/asio/asio/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/websocketpp>

      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/rapidjson/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/RtMidi17>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/oscpack>

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
message(STATUS "OSSIA - Dataflow: ${OSSIA_DATAFLOW}")
message(STATUS "OSSIA - Editor: ${OSSIA_EDITOR}")
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
if(NOT OSSIA_SANITIZE AND NOT OSSIA_DISABLE_COTIRE AND NOT DISABLE_COTIRE )
cotire(ossia)
endif()

message(STATUS "Jack_INCLUDE_DIR : ${Jack_INCLUDE_DIR}")
