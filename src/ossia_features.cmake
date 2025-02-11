include(WholeArchive)

target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:magic_enum::magic_enum>)

if(TARGET Boost::boost)
  target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:Boost::boost>)
elseif(TARGET boost)
  target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:boost>)
endif()

### Core language features ###
include(CheckCXXSourceCompiles)
check_cxx_source_compiles(
[=[
#include <shared_mutex>

int main(){ std::shared_mutex t; }
]=]
    OSSIA_SHARED_MUTEX_AVAILABLE
)

### Protocol setup ###
if(IOS OR EMSCRIPTEN)
  set(OSSIA_PROTOCOL_AUDIO TRUE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_MIDI TRUE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_HTTP FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_WEBSOCKETS FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_SERIAL FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_ARTNET FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_MQTT5 FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_COAP FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_MQTT5 FALSE CACHE INTERNAL "")
endif()

if(NOT OSSIA_QML)
  set(OSSIA_PROTOCOL_HTTP FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_WEBSOCKETS FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_SERIAL FALSE CACHE INTERNAL "")
endif()

if(NOT TARGET libremidi)
  set(OSSIA_PROTOCOL_MIDI OFF CACHE INTERNAL "")
endif()

if(NOT OSSIA_PROTOCOL_MIDI OR NOT OSSIA_PROTOCOL_AUDIO)
  set(OSSIA_DATAFLOW OFF CACHE INTERNAL "")
endif()

if(OSSIA_PROTOCOL_AUDIO)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} AUDIO)
endif()

if(NOT OSSIA_DATAFLOW)
  set(OSSIA_EDITOR OFF)
endif()

# This one is put first because it is *long* to build
# so we want to parallelize it with the rest
if(OSSIA_MATH_EXPRESSION)
  target_sources(ossia PRIVATE ${OSSIA_EXPR_HEADERS} ${OSSIA_EXPR_SRCS})
  target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:exprtk::exprtk>)
endif()

if(OSSIA_PROTOCOL_MIDI)
  target_sources(ossia PRIVATE ${OSSIA_MIDI_SRCS} ${OSSIA_MIDI_HEADERS})
  target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:libremidi>)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} MIDI)

  if(OSSIA_ENABLE_JACK)
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:jack::jack>)
  endif()
endif()

if(OSSIA_PROTOCOL_OSC)
  target_sources(ossia PRIVATE ${OSSIA_OSC_SRCS} ${OSSIA_OSC_HEADERS})
  target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:oscpack::oscpack>)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} OSC)
endif()

if(OSSIA_PROTOCOL_MINUIT)
  target_sources(ossia PRIVATE ${OSSIA_MINUIT_SRCS} ${OSSIA_MINUIT_HEADERS})
  target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:oscpack::oscpack>)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Minuit)
endif()

if(OSSIA_PROTOCOL_OSCQUERY)
  target_sources(ossia PRIVATE ${OSSIA_OSCQUERY_SRCS} ${OSSIA_OSCQUERY_HEADERS})
  target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:oscpack::oscpack>)
  target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:websocketpp::websocketpp>)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} OSCQuery)
endif()

if(OSSIA_PROTOCOL_HTTP)
  find_package(${QT_VERSION} QUIET COMPONENTS Core Qml)
  if(TARGET "${QT_PREFIX}::Qml")
    target_sources(ossia PRIVATE ${OSSIA_HTTP_HEADERS} ${OSSIA_HTTP_SRCS})
    target_link_libraries(ossia PUBLIC ${QT_PREFIX}::Qml)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} HTTP)
    set(OSSIA_PROTOCOL_HTTP TRUE CACHE INTERNAL "" FORCE)
  else()
    set(OSSIA_PROTOCOL_HTTP FALSE CACHE INTERNAL "")
  endif()
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  find_package(${QT_VERSION} QUIET COMPONENTS Core Qml WebSockets)
  if(TARGET "${QT_PREFIX}::WebSockets")
    target_sources(ossia PRIVATE ${OSSIA_WS_CLIENT_HEADERS} ${OSSIA_WS_CLIENT_SRCS})
    target_link_libraries(ossia PUBLIC ${QT_PREFIX}::Qml ${QT_PREFIX}::WebSockets)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} WebSockets)
    set(OSSIA_PROTOCOL_WEBSOCKETS TRUE CACHE INTERNAL "" FORCE)
  else()
    set(OSSIA_PROTOCOL_WEBSOCKETS FALSE CACHE INTERNAL "")
  endif()
endif()

if(OSSIA_PROTOCOL_SERIAL)
  find_package(${QT_VERSION} QUIET COMPONENTS Core SerialPort)
  if(TARGET "${QT_PREFIX}::SerialPort")
    target_sources(ossia PRIVATE ${OSSIA_SERIAL_HEADERS} ${OSSIA_SERIAL_SRCS})
    target_link_libraries(ossia PUBLIC ${QT_PREFIX}::SerialPort)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Serial)
    set(OSSIA_PROTOCOL_SERIAL TRUE CACHE INTERNAL "" FORCE)
  else()
    set(OSSIA_PROTOCOL_SERIAL FALSE CACHE INTERNAL "")
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

if(OSSIA_PROTOCOL_JOYSTICK)
  target_sources(ossia PRIVATE ${OSSIA_JOYSTICK_SRCS} ${OSSIA_JOYSTICK_HEADERS})
  target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:ossia::sdl2>)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Joystick)
endif()

if(OSSIA_PROTOCOL_WIIMOTE)
  target_sources(ossia PRIVATE ${OSSIA_WIIMOTE_SRCS} ${OSSIA_WIIMOTE_HEADERS})
  if(NOT OSSIA_USE_SYSTEM_LIBRARIES)
    target_include_directories(ossia PRIVATE "${WIIUSE_DIR}/src")
  endif()
  target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:wiiuse>)
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Wiimote)
endif()

if (OSSIA_PROTOCOL_ARTNET)
  target_sources(ossia PRIVATE ${OSSIA_ARTNET_SRCS} ${OSSIA_ARTNET_HEADERS})
  set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} artnet)
endif()

if (OSSIA_PROTOCOL_LIBMAPPER)
    target_sources(ossia PRIVATE ${OSSIA_LIBMAPPER_SRCS} ${OSSIA_LIBMAPPER_HEADERS})
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:mapper>)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} libmapper)
endif()

if (OSSIA_PROTOCOL_MQTT5)
    target_sources(ossia PRIVATE ${OSSIA_MQTT5_SRCS} ${OSSIA_MQTT5_HEADERS})
    target_link_libraries(ossia PRIVATE Async::MQTT5)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} mqtt5)
endif()

if (OSSIA_PROTOCOL_COAP)
    target_sources(ossia PRIVATE ${OSSIA_COAP_SRCS} ${OSSIA_COAP_HEADERS})
    target_link_libraries(ossia PRIVATE libcoap::coap-3)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} coap)
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
  target_link_libraries(ossia PUBLIC ${QT_PREFIX}::Core)
  target_include_directories(ossia
    PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt>"
    PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/verdigris/src>
   )
  target_sources(ossia PRIVATE ${OSSIA_QT_HEADERS} ${OSSIA_QT_SRCS})

  if(OSSIA_QML)
    if(TARGET Qt::Core)
      qt_wrap_cpp(cur_moc "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/qml_plugin.hpp" TARGET ossia)
    endif()
    target_sources(ossia PRIVATE ${cur_moc})

    target_sources(ossia PRIVATE ${OSSIA_QTQML_HEADERS} ${OSSIA_QTQML_SRCS})
    target_link_libraries(ossia PUBLIC ${QT_PREFIX}::Gui ${QT_PREFIX}::Qml)
    add_custom_target(ossia-qml-sources SOURCES ${OSSIA_QML_SRCS})
    if(OSSIA_DISABLE_QT_PLUGIN)
      target_compile_definitions(ossia PRIVATE OSSIA_DISABLE_QT_PLUGIN)
    endif()

    if(OSSIA_QML_DEVICE)
      target_sources(ossia PRIVATE ${OSSIA_QT_DEVICE_HEADERS} ${OSSIA_QT_DEVICE_SRCS})
      target_link_libraries(ossia PUBLIC ${QT_PREFIX}::Quick)
    endif()

    if(OSSIA_QML_SCORE)
      target_sources(ossia PRIVATE ${OSSIA_QT_SCORE_HEADERS} ${OSSIA_QT_SCORE_SRCS})
      target_link_libraries(ossia PUBLIC ${QT_PREFIX}::Quick)
    endif()
  endif()
endif()

if(OSSIA_DNSSD)
  target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:Servus_iface>)
endif()

if(OSSIA_DATAFLOW)
  if(NOT CMAKE_SYSTEM_NAME MATCHES Emscripten)
    set(OSSIA_PARALLEL 1)
  endif()

  target_sources(ossia PRIVATE
    ${OSSIA_DATAFLOW_HEADERS}
    ${OSSIA_DATAFLOW_SRCS}
  )

  target_include_directories(ossia PUBLIC
    $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/Flicks>
  )

  target_link_libraries(ossia
    PUBLIC
      $<BUILD_INTERFACE:libremidi::libremidi>
    PRIVATE
      $<BUILD_INTERFACE:websocketpp::websocketpp>
  )

  # JACK support
  if(OSSIA_ENABLE_JACK)
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:jack::jack>)
  endif()

  # PortAudio support
  if(OSSIA_ENABLE_PORTAUDIO)
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:PortAudio::PortAudio>)
  endif()

  # PipeWire
  if(OSSIA_ENABLE_PIPEWIRE)
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:pipewire::pipewire>)
  endif()

  #SDL support
  if(OSSIA_ENABLE_SDL)
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:ossia::sdl2>)
  endif()

  if(OSSIA_ENABLE_LIBSAMPLERATE)
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:SampleRate::samplerate>)
  endif()

  if(OSSIA_ENABLE_RUBBERBAND)
    target_link_libraries(ossia PRIVATE $<BUILD_INTERFACE:rubberband>)
  endif()

  # FFT support
  if(OSSIA_ENABLE_FFT)
    if(OSSIA_ENABLE_FFTW)
      target_link_libraries(ossia PRIVATE
        $<BUILD_INTERFACE:fftw::fftw3>
      )
    elseif(OSSIA_ENABLE_KFR AND KFR_ENABLE_DFT) # defined in kfr/CMakeLists.txt
      target_link_libraries(ossia PRIVATE
        "$<BUILD_INTERFACE:kfr>"
      )
      link_whole_archive(ossia PRIVATE kfr_dft)
    endif()
    target_sources(ossia PRIVATE ${OSSIA_FFT_HEADERS} ${OSSIA_FFT_SRCS})
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

  if(OSSIA_GFX)
    target_sources(ossia PRIVATE ${OSSIA_GFX_HEADERS} ${OSSIA_GFX_SRCS})
  endif()

  if(OSSIA_EDITOR)
    target_sources(ossia PRIVATE ${OSSIA_EDITOR_HEADERS} ${OSSIA_EDITOR_SRCS})

    target_sources(ossia PRIVATE ${OSSIA_EXECLOG_HEADERS} ${OSSIA_EXECLOG_SRCS})
  endif()
elseif(NOT OSSIA_SCENARIO_DATAFLOW)
  # minimal build
  target_sources(ossia PRIVATE ${OSSIA_EDITOR_HEADERS} ${OSSIA_EDITOR_SRCS})
endif()

set_target_properties(ossia PROPERTIES OSSIA_PROTOCOLS "${OSSIA_PROTOCOLS}")

