if(OSSIA_SUBMODULE_AUTOUPDATE)
  message(STATUS "Update general OSSIA dependencies :")
  set(OSSIA_SUBMODULES
      GSL chobo-shl hopscotch-map
      nano-signal-slot brigand whereami
      rapidjson readerwriterqueue websocketpp
      asio variant spdlog fmt
      SmallFunction
      Servus
      bitset2
      concurrentqueue tbb
      exprtk
      flat_hash_map
      multi_index
      frozen
      weakjack
      verdigris
      flat
      )

  execute_process(COMMAND git submodule sync --recursive
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

  foreach(submodule ${OSSIA_SUBMODULES})
      message(" -> ${OSSIA_3RDPARTY_FOLDER}/${submodule}")
      execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/${submodule}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  endforeach()

  if(OSSIA_DNSSD)
    message(" -> ${OSSIA_3RDPARTY_FOLDER}/Servus")
    execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/Servus
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(OSSIA_PROTOCOL_MIDI)
    message(" -> ${OSSIA_3RDPARTY_FOLDER}/RtMidi17")
    execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/RtMidi17
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()


  if (OSSIA_PROTOCOL_OSC OR OSSIA_PROTOCOL_MINUIT OR OSSIA_PROTOCOL_OSCQUERY)
    message(" -> ${OSSIA_3RDPARTY_FOLDER}/oscpack")
    execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/oscpack
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  if(OSSIA_PROTOCOL_ARTNET)
      message(" -> ${OSSIA_3RDPARTY_FOLDER}/libartnet")
      execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/libartnet
                      WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  endif()

  message(" -> ${PROJECT_SOURCE_DIR}/CMake/cmake-modules")
  execute_process(COMMAND git submodule update --init -- ${PROJECT_SOURCE_DIR}/CMake/cmake-modules
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

  if(OSSIA_PROTOCOL_WIIMOTE)
    message(" -> ${OSSIA_3RDPARTY_FOLDER}/wiiuse")
    execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/wiiuse
                    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  endif()

  message(STATUS "...done")
  set(OSSIA_SUBMODULE_AUTOUPDATE OFF CACHE BOOL "Auto update submodule" FORCE)
endif()

# Download various dependencies
if(WIN32)
  message(STATUS "Downloading audio sdk to ${OSSIA_3RDPARTY_FOLDER}/win-audio-sdk.zip")
  if("${OSSIA_SDK}" STREQUAL "")
    set(OSSIA_SDK "ossia-sdk" CACHE INTERNAL "")
  endif()

  if (NOT EXISTS "${OSSIA_SDK}")
    file(MAKE_DIRECTORY ${OSSIA_SDK})
    file(DOWNLOAD
      https://github.com/OSSIA/sdk/releases/download/sdk10/win-audio-sdk.zip
      ${OSSIA_SDK}/win-audio-sdk.zip)

    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xzf win-audio-sdk.zip
      WORKING_DIRECTORY ${OSSIA_SDK})
  endif()
endif()

set(BOOST_MINOR 69)
if(ANDROID)
  set(Boost_FOUND True)
  include_directories("/opt/boost_1_${BOOST_MINOR}_0")
else()
  find_package(Boost 1.${BOOST_MINOR} QUIET)
  if ( NOT Boost_FOUND )
    set(BOOST_VERSION "boost_1_${BOOST_MINOR}_0")
    if ( NOT EXISTS "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/")

      if(WIN32)
        message(STATUS "Downloading boost to ${OSSIA_3RDPARTY_FOLDER}/boost.zip")

        file(DOWNLOAD
          https://dl.bintray.com/boostorg/release/1.${BOOST_MINOR}.0/source/${BOOST_VERSION}.zip
          ${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}.zip)

        execute_process(
          COMMAND ${CMAKE_COMMAND} -E tar xzf ${BOOST_VERSION}.zip
          WORKING_DIRECTORY ${OSSIA_3RDPARTY_FOLDER})

      else()
        message(STATUS "Downloading boost to ${OSSIA_3RDPARTY_FOLDER}/boost.tar.gz")

        file(DOWNLOAD
          https://dl.bintray.com/boostorg/release/1.${BOOST_MINOR}.0/source/${BOOST_VERSION}.tar.gz
          ${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}.tar.gz)

        execute_process(
          COMMAND ${CMAKE_COMMAND} -E tar xzf ${BOOST_VERSION}.tar.gz
          WORKING_DIRECTORY ${OSSIA_3RDPARTY_FOLDER})

      endif()
    endif()
    set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/" CACHE INTERNAL "")
    set(Boost_INCLUDE_DIR ${BOOST_ROOT})
    find_package(Boost REQUIRED)
  endif()

  add_library(boost INTERFACE IMPORTED)
  set_property(TARGET boost PROPERTY
               INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
endif()

if(OSSIA_PROTOCOL_MIDI)
  set(RTMIDI17_HEADER_ONLY 1 CACHE "" INTERNAL)
  set(WEAKJACK_FOLDER "${OSSIA_3RDPARTY_FOLDER}")
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/RtMidi17" EXCLUDE_FROM_ALL)
endif()

if(OSSIA_DATAFLOW)
  set(TBB_LINKAGE "STATIC" CACHE "" INTERNAL)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/tbb" EXCLUDE_FROM_ALL)
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
