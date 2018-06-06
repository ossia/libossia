set(BOOST_MINOR 67)
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
    set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/" )
    find_package(Boost REQUIRED)
  endif()

  add_library(boost INTERFACE IMPORTED)
  set_property(TARGET boost PROPERTY
               INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES Emscripten AND NOT ANDROID)
  message(STATUS "Update OSSIA Midi Protocol dependency : RtMidi17")
  execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/RtMidi17
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  set(RTMIDI17_HEADER_ONLY 1 CACHE "" INTERNAL)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/RtMidi17" EXCLUDE_FROM_ALL)
endif()

if (OSSIA_PROTOCOL_OSC OR 1) # FIXME is OSC really mandatory ?
  message(STATUS "Update OSSIA OSC Protocol dependency : oscpack")
  execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/oscpack
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)
endif()

if(OSSIA_DNSSD)
  message(STATUS "Update OSSIA DNSSD dependency : servus")
  execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/Servus
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_subdirectory(3rdparty/Servus EXCLUDE_FROM_ALL)
endif()
