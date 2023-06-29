set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_MODULE_PATH}"
)

if(OSSIA_SDK)
  list(APPEND CMAKE_PREFIX_PATH "${OSSIA_SDK}/portaudio/lib/cmake/portaudio")
endif()

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
  cmake_policy(VERSION 3.18)
endif()

project(ossia C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
find_package(Threads)

if(NOT QT_VERSION)
  set(QT_VERSION Qt6 6.4)
  set(QT_PREFIX Qt6)
endif()
find_package(${QT_VERSION} COMPONENTS Core)

# Options
## FIXME : the following variable doesn't have a type
## BUT cmake seems to suffer from a bug here
## with `set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty" CACHE PATH "" INTERNAL)`
## the variable looks like : `/tmp/DpvCam/3rdparty/libossia/3rdparty;CACHE;STRING;;INTERNAL`
## which is a list instead of a single PATH value
## at least with CMake 15.2
set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty" CACHE INTERNAL "")
include(OssiaOptions)

# Dependencies
include(OssiaDeps)

# Configuration
include(OssiaConfiguration)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0069 NEW)

# Main library
add_subdirectory(src)

# Examples & tests
if(OSSIA_EXAMPLES)
    add_subdirectory(examples)
endif()

if(OSSIA_TESTING)
  include(CTest)
  if(OSSIA_USE_SYSTEM_LIBRARIES)
    find_package(Catch2 CONFIG REQUIRED)
    if(NOT TARGET Catch2::Catch2WithMain)
      add_library(Catch2WithMain INTERFACE)
      add_library(Catch2::Catch2WithMain ALIAS Catch2WithMain)
      target_compile_definitions(Catch2WithMain INTERFACE CATCH_CONFIG_MAIN)
      target_link_libraries(Catch2WithMain INTERFACE Catch2::Catch2)
    endif()
  else()
    set(CATCH_BUILD_STATIC_LIBRARY 1)
    set(CATCH_BUILD_STATIC_LIBRARY 1 CACHE INTERNAL "" FORCE)
    add_subdirectory(3rdparty/Catch2)
  endif()
  add_subdirectory(tests)
endif()

if(OSSIA_GENVAR)
  add_subdirectory(tools/gen_variant)
endif()
