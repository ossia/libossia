cmake_minimum_required(VERSION 3.17)

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
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@executable_path;")
endif()

if(OSSIA_QT)
  find_package(Qt5 5.6 QUIET ${OSSIA_QT_REQUIRED} COMPONENTS Core OPTIONAL_COMPONENTS Gui Qml)

  if(OSSIA_QML_DEVICE)
    find_package(Qt5 5.6 QUIET ${OSSIA_QT_REQUIRED} COMPONENTS Quick)
  endif()

  if(NOT "${Qt5Core_FOUND}")
    set(OSSIA_QT OFF)
  endif()
  if("${Qt5Qml_FOUND}")
    set(OSSIA_QML ON)
  else()
    set(OSSIA_QML OFF)
  endif()
  if(NOT "${Qt5Quick_FOUND}")
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
message(STATUS "OSSIA - Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "OSSIA - Sanitize: ${OSSIA_SANITIZE}")
message(STATUS "OSSIA - Tidy: ${OSSIA_TIDY}")
message(STATUS "OSSIA - Static: ${OSSIA_STATIC}")
message(STATUS "OSSIA - Coverage: ${OSSIA_COVERAGE}")
message(STATUS "OSSIA - Examples: ${OSSIA_EXAMPLES}")
message(STATUS "OSSIA - Tests: ${OSSIA_TESTING}")
message(STATUS "OSSIA - CI: ${OSSIA_CI}")
message(STATUS "OSSIA - Framework: ${OSSIA_FRAMEWORK}")
message(STATUS "OSSIA - Dataflow: ${OSSIA_DATAFLOW}")
message(STATUS "OSSIA - Editor: ${OSSIA_EDITOR}")
message(STATUS "OSSIA - Protocols: ${OSSIA_PROTOCOLS}")
message(STATUS "OSSIA - Zeroconf: ${KF5DNSSD_FOUND}")
message(STATUS "OSSIA - LTO: ${OSSIA_LTO}")
message(STATUS "OSSIA - OSX Architectures: ${OSSIA_OSX_FAT_LIBRARIES}")
message(STATUS "OSSIA - OSX Retrocompatibility: ${OSSIA_OSX_RETROCOMPATIBILITY}")
message("")
message(STATUS "OSSIA - bindings ----------")
message(STATUS "OSSIA - PureData: ${OSSIA_PD}")
message(STATUS "OSSIA - Max: ${OSSIA_MAX}")
message(STATUS "OSSIA - Python: ${OSSIA_PYTHON}")
message(STATUS "OSSIA - Unity3d: ${OSSIA_UNITY3D}")
message(STATUS "OSSIA - Java: ${OSSIA_JAVA}")
message(STATUS "OSSIA - Qt: ${OSSIA_QT}")
message(STATUS "OSSIA - C: ${OSSIA_C}")
message(STATUS "OSSIA - CPP: ${OSSIA_CPP}")
message(STATUS "Jack_INCLUDE_DIR : ${Jack_INCLUDE_DIR}")

if(OSSIA_PCH)
  target_precompile_headers(ossia PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/ossia/prefix.hpp>"
  )
endif()

