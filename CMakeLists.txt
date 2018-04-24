set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake")
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(OSSIA CXX)
find_package(Threads)
find_package(Qt5 5.3 COMPONENTS Core)

# Configuration

# activate select policies
#cmake_policy(VERSION 3.9)
set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")
include(OssiaConfiguration)
cmake_policy(SET CMP0069 NEW)

# Dependencies

if(ANDROID)
  set(Boost_FOUND True)
  include_directories("/opt/boost_1_64_0")
else()
  find_package(Boost QUIET)
  if ( NOT Boost_FOUND )
    set(BOOST_VERSION "boost_1_66_0")
    if ( NOT EXISTS "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/")

      if(WIN32)
        message(STATUS "Downloading boost to ${OSSIA_3RDPARTY_FOLDER}/boost.zip")
        
        file(DOWNLOAD
          https://dl.bintray.com/boostorg/release/1.66.0/source/${BOOST_VERSION}.zip
          ${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}.zip)

        execute_process(
          COMMAND ${CMAKE_COMMAND} -E tar xzf ${BOOST_VERSION}.zip
          WORKING_DIRECTORY ${OSSIA_3RDPARTY_FOLDER})

      else()
        message(STATUS "Downloading boost to ${OSSIA_3RDPARTY_FOLDER}/boost.tar.gz")
        
        file(DOWNLOAD
          https://dl.bintray.com/boostorg/release/1.66.0/source/${BOOST_VERSION}.tar.gz
          ${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}.tar.gz)
        
        execute_process(
          COMMAND ${CMAKE_COMMAND} -E tar xzf ${BOOST_VERSION}.tar.gz
          WORKING_DIRECTORY ${OSSIA_3RDPARTY_FOLDER})

      endif()
    endif()
    set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/" )
    set(BOOST_INCLUDEDIR "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/boost")
    find_package(Boost REQUIRED)
  endif()
  add_library(boost INTERFACE IMPORTED)
  set_property(TARGET boost PROPERTY
               INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES Emscripten AND NOT ANDROID)
  if(OSSIA_PROTOCOL_MIDI)
    message(STATUS "Update OSSIA Midi Protocol dependency : ModernMIDI")
    execute_process(COMMAND git submodule update --init -- ${OSSIA_3RDPARTY_FOLDER}/ModernMIDI
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/ModernMIDI" EXCLUDE_FROM_ALL)
  endif()
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

# Main library
find_package(PortAudio)
add_definitions(-DFMT_HEADER_ONLY=1)
add_subdirectory(OSSIA)

if(ANDROID)
  return()
endif()

# Bindings
if(OSSIA_PD)
    add_subdirectory(OSSIA/ossia-pd)
endif()

if(OSSIA_MAX)
    add_subdirectory(OSSIA/ossia-max)
endif()

if(NOT WIN32)
if(OSSIA_PYTHON)
    add_subdirectory(OSSIA/ossia-python OSSIA/ossia-python/pyossia)
endif()
endif()

# Examples & tests
if(OSSIA_EXAMPLES)
    add_subdirectory(Documentation/Examples)
endif()

if(OSSIA_TESTING)
    enable_testing()
    add_subdirectory(Tests)
endif()

if(OSSIA_GENVAR)
    add_subdirectory(tools/gen_variant)
endif()
