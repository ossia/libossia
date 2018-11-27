set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake"
  "${CMAKE_MODULE_PATH}"

  PARENT_SCOPE
)

set(CMAKE_PREFIX_PATH
  "${OSSIA_SDK}/SDL2/cmake"
  "${OSSIA_SDK}/portaudio/lib/cmake"
  "${CMAKE_PREFIX_PATH}"

  PARENT_SCOPE
)

if(APPLE AND EXISTS /usr/local/opt/qt5)
  list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()

cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(OSSIA CXX)
find_package(Threads)
find_package(Qt5 5.3 COMPONENTS Core)

# Configuration

# activate select policies
#cmake_policy(VERSION 3.9)
set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake-modules/LICENSE")
  execute_process(COMMAND git submodule update --init -- ${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake-modules
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

include(OssiaConfiguration)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0069 NEW)

# Dependencies
include(CMake/OssiaDeps.cmake)

# Main library
find_package(PortAudio)
add_definitions(-DFMT_HEADER_ONLY=1)
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()
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

if(OSSIA_JAVA)
  add_subdirectory(OSSIA/ossia-java)
endif()

if(OSSIA_NODEJS)
  add_subdirectory(OSSIA/ossia-node)
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
