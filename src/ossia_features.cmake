if(TARGET Boost::boost)
  target_link_libraries(ossia PUBLIC Boost::boost)
endif()

### Protocol setup ###
# These two are always here:
if(IOS OR CMAKE_SYSTEM_NAME MATCHES Emscripten)
  set(OSSIA_PROTOCOL_AUDIO FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_MIDI FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_HTTP FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_WEBSOCKETS FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_SERIAL FALSE CACHE INTERNAL "")
endif()

if(NOT OSSIA_QML)
  set(OSSIA_PROTOCOL_HTTP FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_WEBSOCKETS FALSE CACHE INTERNAL "")
  set(OSSIA_PROTOCOL_SERIAL FALSE CACHE INTERNAL "")
endif()

if(NOT TARGET RtMidi17)
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
    set(OSSIA_PROTOCOL_HTTP FALSE CACHE INTERNAL "")
  endif()
endif()

if(OSSIA_PROTOCOL_WEBSOCKETS)
  find_package(Qt5 5.6 QUIET COMPONENTS Core Qml WebSockets)
  if(${Qt5WebSockets_FOUND})
    target_sources(ossia PRIVATE ${OSSIA_WS_CLIENT_HEADERS} ${OSSIA_WS_CLIENT_SRCS})
    target_link_libraries(ossia PUBLIC Qt5::Qml Qt5::WebSockets)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} WebSockets)
  else()
    set(OSSIA_PROTOCOL_WEBSOCKETS FALSE CACHE INTERNAL "")
  endif()
endif()

if(OSSIA_PROTOCOL_SERIAL)
  find_package(Qt5 5.6 QUIET COMPONENTS Core SerialPort)
  if(${Qt5SerialPort_FOUND})
    target_sources(ossia PRIVATE ${OSSIA_SERIAL_HEADERS} ${OSSIA_SERIAL_SRCS})
    target_link_libraries(ossia PUBLIC Qt5::SerialPort)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Serial)
  else()
    set(OSSIA_PROTOCOL_SERIAL FALSE CACHE INTERNAL "")
  endif()
endif()

if(OSSIA_PROTOCOL_PHIDGETS)
  find_package(Phidgets)

  if(NOT "${LIBPHIDGETS_FOUND}")
    set(OSSIA_PROTOCOL_PHIDGETS FALSE CACHE INTERNAL "")
  endif()

  if(UNIX AND NOT APPLE)
    find_package(USB)
    if(NOT "${LIBUSB_FOUND}")
      set(OSSIA_PROTOCOL_PHIDGETS FALSE CACHE INTERNAL "")
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
    set(OSSIA_PROTOCOL_LEAPMOTION FALSE CACHE INTERNAL "")
  endif()
endif()

if(OSSIA_PROTOCOL_JOYSTICK)
  find_package(SDL2 CONFIG)
  if(TARGET SDL2::SDL2-static)
    target_sources(ossia PRIVATE ${OSSIA_JOYSTICK_SRCS} ${OSSIA_JOYSTICK_HEADERS})
    target_link_libraries(ossia PRIVATE SDL2::SDL2-static)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Joystick)
  elseif(TARGET SDL2::SDL2)
    target_sources(ossia PRIVATE ${OSSIA_JOYSTICK_SRCS} ${OSSIA_JOYSTICK_HEADERS})
    target_link_libraries(ossia PRIVATE SDL2::SDL2)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Joystick)
  elseif(SDL2_LIBRARIES AND SDL2_INCLUDE_DIRS)
    target_sources(ossia PRIVATE ${OSSIA_JOYSTICK_SRCS} ${OSSIA_JOYSTICK_HEADERS})
    target_include_directories(ossia PRIVATE "${SDL2_INCLUDE_DIRS}")
    target_link_libraries(ossia PRIVATE "${SDL2_LIBRARIES}")
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} Joystick)
  else()
    set(OSSIA_PROTOCOL_JOYSTICK FALSE CACHE "" INTERNAL FORCE)
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

if (OSSIA_PROTOCOL_LIBMAPPER)
    target_sources(ossia PRIVATE ${OSSIA_LIBMAPPER_SRCS} ${OSSIA_LIBMAPPER_HEADERS})
    target_link_libraries(ossia PRIVATE mapper)
    set(OSSIA_PROTOCOLS ${OSSIA_PROTOCOLS} libmapper)
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
  target_link_libraries(ossia PUBLIC Qt5::Core)

  disable_qt_plugins(ossia)
  target_include_directories(ossia
    PRIVATE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt>"
    PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/verdigris/src>
   )
  target_sources(ossia PRIVATE ${OSSIA_QT_HEADERS} ${OSSIA_QT_SRCS})

  if(OSSIA_QML)
    qt5_wrap_cpp(cur_moc "${CMAKE_CURRENT_SOURCE_DIR}/ossia-qt/qml_plugin.hpp" TARGET ossia)
    target_sources(ossia PRIVATE ${cur_moc})

    target_sources(ossia PRIVATE ${OSSIA_QTQML_HEADERS} ${OSSIA_QTQML_SRCS})
    target_link_libraries(ossia PUBLIC Qt5::Gui Qt5::Qml)
    add_custom_target(ossia-qml-sources SOURCES ${OSSIA_QML_SRCS})
    if(OSSIA_DISABLE_QT_PLUGIN)
      target_compile_definitions(ossia PRIVATE OSSIA_DISABLE_QT_PLUGIN)
    endif()

    if(OSSIA_QML_DEVICE)
      target_sources(ossia PRIVATE ${OSSIA_QT_DEVICE_HEADERS} ${OSSIA_QT_DEVICE_SRCS})
      target_link_libraries(ossia PUBLIC Qt5::Quick)
    endif()

    if(OSSIA_QML_SCORE)
      target_sources(ossia PRIVATE ${OSSIA_QT_SCORE_HEADERS} ${OSSIA_QT_SCORE_SRCS})
      target_link_libraries(ossia PUBLIC Qt5::Quick)
    endif()
  endif()
endif()

if(OSSIA_DNSSD)
  target_link_libraries(ossia PRIVATE Servus_iface)
endif()

if(OSSIA_DATAFLOW)
  set(OSSIA_PARALLEL 1)

  target_include_directories(ossia PUBLIC
    $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/Flicks>
    $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/cpp-taskflow>
  )

  target_sources(ossia PRIVATE ${OSSIA_DATAFLOW_HEADERS} ${OSSIA_DATAFLOW_SRCS})

  ossia_link_jack()
  if(TARGET portaudio_static)
    target_link_libraries(ossia PUBLIC portaudio_static)
  elseif(TARGET portaudio)
    target_link_libraries(ossia PUBLIC portaudio)
  else()
    find_library(PORTAUDIO_LIBRARY NAMES libportaudio.so portaudio)
    if(PORTAUDIO_LIBRARY)
      target_link_libraries(ossia PUBLIC "${PORTAUDIO_LIBRARY}")
    endif()
  endif()

  set(SDL_BUILDING_LIBRARY TRUE)

  find_package(SDL2 CONFIG)
  if(TARGET SDL2::SDL2-static)
    target_link_libraries(ossia PRIVATE SDL2::SDL2-static)
  elseif(TARGET SDL2::SDL2)
    target_link_libraries(ossia PRIVATE SDL2::SDL2)
  else()
    # Used for audio support in emscripten
    find_package(SDL)
    if(SDL_FOUND)
      target_include_directories(ossia PUBLIC ${SDL_INCLUDE_DIR})
      target_link_libraries(ossia PUBLIC ${SDL_LIBRARY})
    endif()
  endif()


  target_link_libraries(ossia PUBLIC samplerate)
  target_link_libraries(ossia PUBLIC rubberband)
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

if(OSSIA_GFX)
  target_sources(ossia PRIVATE ${OSSIA_GFX_HEADERS} ${OSSIA_GFX_SRCS})
endif()

if(OSSIA_EDITOR)
  target_sources(ossia PRIVATE ${OSSIA_EDITOR_HEADERS} ${OSSIA_EDITOR_SRCS})
endif()

set_target_properties(ossia PROPERTIES OSSIA_PROTOCOLS "${OSSIA_PROTOCOLS}")
