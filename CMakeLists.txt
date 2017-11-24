set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

cmake_minimum_required(VERSION 3.5)
project(OSSIA CXX)

# Configuration

# activate select policies

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0025 NEW)
cmake_policy(SET CMP0026 NEW)
cmake_policy(SET CMP0038 NEW)
cmake_policy(SET CMP0039 NEW)
cmake_policy(SET CMP0040 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0045 NEW)
cmake_policy(SET CMP0046 NEW)
cmake_policy(SET CMP0049 NEW)
cmake_policy(SET CMP0050 NEW)
cmake_policy(SET CMP0051 NEW)
cmake_policy(SET CMP0053 NEW)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0055 NEW)
cmake_policy(SET CMP0063 NEW)
cmake_policy(SET CMP0071 NEW)

include(OssiaConfiguration)

# Dependencies
set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")
find_package(Qt5 5.3 COMPONENTS Core)
if(ANDROID)
  set(Boost_FOUND True)
  include_directories("/opt/boost_1_64_0")
else()
  find_package(Boost REQUIRED)
  add_library(boost INTERFACE IMPORTED)
  set_property(TARGET boost PROPERTY
               INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
endif()
if(NOT CMAKE_SYSTEM_NAME MATCHES Emscripten AND NOT ANDROID)
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/ModernMIDI" EXCLUDE_FROM_ALL)
endif()

add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)
if(OSSIA_DNSSD)
  add_subdirectory(3rdparty/Servus EXCLUDE_FROM_ALL)
endif()

# Main library
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
