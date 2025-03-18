find_package(Git)

if(Git_FOUND AND OSSIA_SUBMODULE_AUTOUPDATE)
  message(STATUS "Update general libossia dependencies :")
  set(OSSIA_SUBMODULES
      concurrentqueue
      compile-time-regular-expressions
      Flicks
      fmt
      magic_enum
      mdspan
      nano-signal-slot
      rapidfuzz-cpp
      rapidjson
      readerwriterqueue
      re2
      rnd
      Servus
      SmallFunction
      span
      spdlog
      tuplet
      unordered_dense
      verdigris
      websocketpp
      whereami
      ../cmake/cmake-modules
      ios-cmake
  )

  if(OSSIA_ENABLE_JACK)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} weakjack)
  endif()

  if(OSSIA_DATAFLOW)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} dno dr_libs exprtk libsamplerate PerlinNoise rubberband)
  endif()

  if(OSSIA_ENABLE_FFT OR OSSIA_PROTOCOL_AUDIO OR OSSIA_DATAFLOW)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} kfr)
  endif()

  if(OSSIA_DNSSD)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} Servus)
  endif()

  if(OSSIA_PROTOCOL_MIDI)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} libremidi)
  endif()

  if (OSSIA_PROTOCOL_OSC OR OSSIA_PROTOCOL_MINUIT OR OSSIA_PROTOCOL_OSCQUERY)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} oscpack)
  endif()

  if(OSSIA_PROTOCOL_MQTT5)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} async-mqtt5)
  endif()

  if(OSSIA_PROTOCOL_WIIMOTE)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} wiiuse)
  endif()

  if(OSSIA_TESTING)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} Catch2)
  endif()

  execute_process(COMMAND ${GIT_EXECUTABLE} submodule sync --recursive
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                  COMMAND_ERROR_IS_FATAL ANY)

  foreach(submodule ${OSSIA_SUBMODULES})
      message(" -> ${OSSIA_3RDPARTY_FOLDER}/${submodule}")
      execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${OSSIA_3RDPARTY_FOLDER}/${submodule}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                      COMMAND_ERROR_IS_FATAL ANY)
  endforeach()

  message(STATUS "...done")
  set(OSSIA_SUBMODULE_AUTOUPDATE OFF CACHE BOOL "Auto update submodule" FORCE)
endif()

# Download various dependencies
include(deps/boost)
include(deps/concurrentqueue)
include(deps/ctre)
include(deps/flicks)
include(deps/fmt)
include(deps/magic_enum)
include(deps/mdspan)
include(deps/nano-signal-slot)
include(deps/rapidfuzz)
include(deps/rapidjson)
include(deps/re2)
include(deps/readerwriterqueue)
include(deps/rnd)
include(deps/smallfunction)
include(deps/span)
include(deps/spdlog)
include(deps/tuplet)
include(deps/unordered_dense)
include(deps/verdigris)
include(deps/websocketpp)

if(OSSIA_PROTOCOL_COAP)
  include(deps/coap)
  if(NOT TARGET libcoap::coap-3)
    set(OSSIA_PROTOCOL_COAP FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()

if(OSSIA_PROTOCOL_MQTT5)
  include(deps/mqtt)
  if(NOT TARGET Async::MQTT5)
    set(OSSIA_PROTOCOL_MQTT5 FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()

if(OSSIA_PROTOCOL_MIDI)
  include(deps/libremidi)
endif()

if(OSSIA_ENABLE_FFT)
  if(OSSIA_ENABLE_FFTW)
    include(deps/fftw)

    if(NOT TARGET fftw::fftw3)
      set(OSSIA_ENABLE_FFTW FALSE CACHE INTERNAL "" FORCE)
    endif()

  endif()
endif()

if(OSSIA_ENABLE_FFT OR OSSIA_PROTOCOL_AUDIO OR OSSIA_DATAFLOW)
   include(deps/kfr)
   if(NOT TARGET kfr)
     set(OSSIA_ENABLE_KFR FALSE CACHE INTERNAL "" FORCE)
   endif()
endif()

if(NOT OSSIA_ENABLE_FFTW AND NOT TARGET kfr_dft)
  set(OSSIA_FFT NAIVE CACHE INTERNAL "")
  set(OSSIA_FFT_NAIVE 1 CACHE INTERNAL "")
endif()

if(OSSIA_DATAFLOW)
  include(deps/dno)
  include(deps/dr_libs)
  include(deps/exprtk)
  include(deps/perlinnoise)
  if(OSSIA_ENABLE_LIBSAMPLERATE)
    include(deps/samplerate) # comes before as rubberband depends on it
    if(NOT TARGET SampleRate::samplerate)
      set(OSSIA_ENABLE_SAMPLERATE FALSE CACHE INTERNAL "" FORCE)
    endif()
  endif()

  if(OSSIA_ENABLE_RUBBERBAND)
    include(deps/rubberband)
    if(NOT TARGET rubberband)
      set(OSSIA_ENABLE_RUBBERBAND FALSE CACHE INTERNAL "" FORCE)
    endif()
  endif()
endif()

if(OSSIA_PROTOCOL_OSC OR OSSIA_PROTOCOL_MINUIT OR OSSIA_PROTOCOL_OSCQUERY)
  include(deps/oscpack)
endif()

if(OSSIA_DNSSD)
  include(deps/servus)
endif()

if(OSSIA_ENABLE_JACK)
  include(deps/jack)

  if(NOT TARGET jack::jack)
    set(OSSIA_ENABLE_JACK FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()

if(OSSIA_ENABLE_PORTAUDIO)
  include(deps/portaudio)

  if(NOT TARGET PortAudio::PortAudio)
    set(OSSIA_ENABLE_PORTAUDIO FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()

if(OSSIA_ENABLE_PIPEWIRE)
  include(deps/pipewire)

  if(NOT TARGET pipewire::pipewire)
    set(OSSIA_ENABLE_PIPEWIRE FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()

if(OSSIA_ENABLE_SDL)
  include(deps/sdl)

  if(NOT TARGET ossia::sdl2)
    set(OSSIA_ENABLE_SDL FALSE CACHE "" INTERNAL FORCE)
    set(OSSIA_PROTOCOL_JOYSTICK FALSE CACHE "" INTERNAL FORCE)
  endif()
endif()

if(OSSIA_PROTOCOL_WIIMOTE)
  include(deps/wiiuse)

  if(NOT TARGET wiiuse)
    set(OSSIA_PROTOCOL_WIIMOTE FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()

if(OSSIA_PROTOCOL_LIBMAPPER)
  find_package(Libmapper REQUIRED)

  if(NOT TARGET Libmapper)
    set(OSSIA_PROTOCOL_LIBMAPPER FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()
