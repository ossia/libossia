if(OSSIA_SUBMODULE_AUTOUPDATE)
  message(STATUS "Update general libossia dependencies :")
  set(OSSIA_SUBMODULES
      asio
      bitset2
      brigand
      chobo-shl
      concurrentqueue
      flat
      flat_hash_map
      Flicks
      fmt
      frozen
      GSL
      hopscotch-map
      multi_index
      nano-signal-slot
      rapidfuzz-cpp
      rapidjson
      readerwriterqueue
      rnd
      Servus
      SmallFunction
      spdlog
      variant
      verdigris
      weakjack
      websocketpp
      whereami
      ../cmake/cmake-modules
  )

  if(OSSIA_DATAFLOW)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} exprtk dr_libs rubberband libsamplerate cpp-taskflow)
  endif()

  if(OSSIA_DNSSD)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} Servus)
  endif()

  if(OSSIA_PROTOCOL_MIDI)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} RtMidi17)
  endif()

  if (OSSIA_PROTOCOL_OSC OR OSSIA_PROTOCOL_MINUIT OR OSSIA_PROTOCOL_OSCQUERY)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} oscpack)
  endif()

  if(OSSIA_PROTOCOL_ARTNET)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} libartnet)
  endif()

  if(OSSIA_PROTOCOL_WIIMOTE)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} wiiuse)
  endif()

  if(OSSIA_TESTING)
    set(OSSIA_SUBMODULES ${OSSIA_SUBMODULES} Catch2)
  endif()

  execute_process(COMMAND git submodule sync --recursive
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

  foreach(submodule ${OSSIA_SUBMODULES})
      message(" -> ${OSSIA_3RDPARTY_FOLDER}/${submodule}")
      execute_process(COMMAND git submodule update --init --recursive -- ${OSSIA_3RDPARTY_FOLDER}/${submodule}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  endforeach()

  message(STATUS "...done")
  set(OSSIA_SUBMODULE_AUTOUPDATE OFF CACHE BOOL "Auto update submodule" FORCE)
endif()

# Download various dependencies
set(BOOST_MINOR_MINIMAL 67)
set(BOOST_MINOR_LATEST 75)
find_package(Boost 1.${BOOST_MINOR_MINIMAL} QUIET)
if (NOT Boost_FOUND)
  set(OSSIA_MUST_INSTALL_BOOST 1 CACHE INTERNAL "")
  set(BOOST_VERSION "boost_1_${BOOST_MINOR_LATEST}_0" CACHE INTERNAL "")
  if ( NOT EXISTS "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/")

    if(WIN32)
      message(STATUS "Downloading boost to ${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}.zip")
      set(BOOST_URL https://github.com/ossia/sdk/releases/download/sdk17/${BOOST_VERSION}.zip)
      set(BOOST_ARCHIVE ${BOOST_VERSION}.zip)
    else()
      message(STATUS "Downloading boost to ${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}.tar.gz")
      set(BOOST_URL https://github.com/ossia/sdk/releases/download/sdk17/${BOOST_VERSION}.tar.gz)
      set(BOOST_ARCHIVE ${BOOST_VERSION}.tar.gz)
    endif()

    file(DOWNLOAD "${BOOST_URL}" "${OSSIA_3RDPARTY_FOLDER}/${BOOST_ARCHIVE}")

  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar xzf "${BOOST_ARCHIVE}"
    WORKING_DIRECTORY "${OSSIA_3RDPARTY_FOLDER}")

  endif()
  set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}" CACHE INTERNAL "")
  set(Boost_INCLUDE_DIR "${BOOST_ROOT}")
  find_package(Boost 1.${BOOST_MINOR_LATEST} REQUIRED)
endif()

add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
             INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}")

if(OSSIA_PROTOCOL_MIDI)
  set(RTMIDI17_EXAMPLES OFF CACHE "" INTERNAL)
  set(RTMIDI17_HEADER_ONLY ON CACHE "" INTERNAL)
  set(WEAKJACK_FOLDER "${OSSIA_3RDPARTY_FOLDER}")
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/RtMidi17" EXCLUDE_FROM_ALL)
endif()

if(OSSIA_DATAFLOW)
  set(_oldmode ${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS 0)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/libsamplerate" EXCLUDE_FROM_ALL)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/rubberband" EXCLUDE_FROM_ALL)
  set(BUILD_SHARED_LIBS ${_oldmode})
endif()

if (OSSIA_PROTOCOL_OSC OR OSSIA_PROTOCOL_MINUIT OR OSSIA_PROTOCOL_OSCQUERY)
  add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)
endif()

if(OSSIA_DNSSD)
  add_subdirectory(3rdparty/Servus EXCLUDE_FROM_ALL)
endif()

if(OSSIA_PROTOCOL_WIIMOTE)
  set(WIIUSE_DIR "${OSSIA_3RDPARTY_FOLDER}/wiiuse")
  add_subdirectory("${WIIUSE_DIR}" wiiuse)

  if(NOT TARGET wiiuse)
    set(OSSIA_PROTOCOL_WIIMOTE FALSE CACHE INTERNAL "" FORCE)
  endif()
endif()

if(OSSIA_PROTOCOL_LIBMAPPER)
    find_package(Libmapper REQUIRED)
endif()

if(NOT (OSSIA_CI AND (UNIX AND NOT APPLE)))
  find_package(PortAudio QUIET)
  if(NOT PortAudio_FOUND)
    find_package(portaudio QUIET)
  endif()
endif()

add_definitions(-DFMT_HEADER_ONLY=1)
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()

include(ExternalProject)
# rapidfuzz
ExternalProject_add(rapidfuzz-cpp
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../3rdparty/rapidfuzz-cpp"
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/rapidfuzz-cpp
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

ExternalProject_Get_property(rapidfuzz-cpp INSTALL_DIR)
set(RAPIDFUZZ_INCLUDE_DIR ${INSTALL_DIR}/include)

