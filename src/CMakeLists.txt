
project(libossia CXX)

### Project options ###
set(ossia_VERSION 1.0.0)

set(OSSIA_PROTOCOLS)
set(CMAKE_AUTOMOC 0)
set(WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)
if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH "${OSSIA_IS_TOP_LEVEL}")
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@executable_path;")
endif()

if(OSSIA_QT)
  find_package(${QT_VERSION} QUIET ${OSSIA_QT_REQUIRED} COMPONENTS Core)
  find_package(${QT_VERSION} QUIET OPTIONAL_COMPONENTS Gui Qml)

  if(TARGET Qt::Core)
    set(QT_VERSION_MAJOR "")
  endif()

  if(OSSIA_QML_DEVICE)
    find_package(${QT_VERSION} QUIET ${OSSIA_QT_REQUIRED} COMPONENTS Quick)
  endif()

  if(TARGET "Qt${QT_VERSION_MAJOR}::Core")
    set(OSSIA_QT ON)
  else()
    set(OSSIA_QT OFF)
  endif()
  if(TARGET "Qt${QT_VERSION_MAJOR}::Qml")
    set(OSSIA_QML ON)
  else()
    set(OSSIA_QML OFF)
  endif()
  if(NOT TARGET "Qt${QT_VERSION_MAJOR}::Core")
    set(OSSIA_QML_DEVICE OFF)
  endif()
  if(NOT DEFINED OSSIA_QML_DEVICE)
    set(OSSIA_QML_DEVICE 1)
  endif()
endif()

### Library ###
include(ossia_sources.cmake)

if(APPLE AND OSSIA_FRAMEWORK)
  add_library(ossia MODULE ${SRCS})
  set_target_properties(ossia PROPERTIES BUNDLE TRUE)
else()
  if(OSSIA_STATIC OR NOT BUILD_SHARED_LIBS)
    add_library(ossia STATIC ${SRCS})
  else()
    add_library(ossia SHARED ${SRCS})
  endif()
endif()

include(ossia_ci.cmake)

include(ossia_features.cmake)

include(ossia_setup.cmake)

if(NOT OSSIA_NO_INSTALL)
  include(ossia_install.cmake)
endif()

include(ossia_tooling.cmake)

include(ossia_bindings.cmake)

### Some status info ###
message(STATUS "libossia - Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "libossia - Static: ${OSSIA_STATIC}")
message(STATUS "libossia - Coverage: ${OSSIA_COVERAGE}")
message(STATUS "libossia - Examples: ${OSSIA_EXAMPLES}")
message(STATUS "libossia - Benchmarks: ${OSSIA_BENCHMARKS}")
message(STATUS "libossia - Tests: ${OSSIA_TESTING}")
message(STATUS "libossia - CI: ${OSSIA_CI}")
message(STATUS "libossia - Framework: ${OSSIA_FRAMEWORK}")
message(STATUS "libossia - Dataflow: ${OSSIA_DATAFLOW}")
message(STATUS "libossia - Editor: ${OSSIA_EDITOR}")
message(STATUS "libossia - Protocols: ${OSSIA_PROTOCOLS}")
message(STATUS "libossia - Zeroconf: ${OSSIA_DNSSD}")
if(APPLE)
  message(STATUS "libossia - OSX Architectures: ${OSSIA_OSX_FAT_LIBRARIES}")
  message(STATUS "libossia - OSX Retrocompatibility: ${OSSIA_OSX_RETROCOMPATIBILITY}")
endif()
message("")
message(STATUS "libossia - bindings ----------")
message(STATUS "libossia - PureData: ${OSSIA_PD}")
message(STATUS "libossia - Max: ${OSSIA_MAX}")
message(STATUS "libossia - Python: ${OSSIA_PYTHON}")
message(STATUS "libossia - Unity3d: ${OSSIA_UNITY3D}")
message(STATUS "libossia - Java: ${OSSIA_JAVA}")
message(STATUS "libossia - Qt: ${OSSIA_QT}")
message(STATUS "libossia - C: ${OSSIA_C}")
message(STATUS "libossia - CPP: ${OSSIA_CPP}")
message(STATUS "Jack_INCLUDE_DIR : ${Jack_INCLUDE_DIR}")

if(OSSIA_PCH)
  target_precompile_headers(ossia PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/ossia/prefix.hpp>"
  )
endif()

