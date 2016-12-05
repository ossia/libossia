cmake_minimum_required(VERSION 3.0)
project("ossia-pd")

if(WIN32)
    message("Pd externals not ready to build yet on Windows")
    return()
endif()

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.4)
set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")

set(PD_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/externals/")
set(PD_MAIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pure-data)
include_directories(${PD_MAIN_PATH}/src)

include_directories(${PD_MAIN_PATH}/src)

add_subdirectory(CicmWrapper)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/CicmWrapper/Sources)

set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty")
include_directories(SYSTEM "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include")

set(PROJECT_SRCS
  src/device.cpp
  src/device.hpp
  src/model.cpp
  src/model.hpp
  src/ossia-pd.hpp
  src/ossia-pd.cpp
  src/parameter.cpp
  src/parameter.hpp
  src/remote.cpp
  src/remote.hpp)

add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})
target_link_libraries(${PROJECT_NAME} CicmWrapper)
target_link_libraries(${PROJECT_NAME} ossia)
target_compile_options(${PROJECT_NAME} -std=c++14)

set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PD_OUTPUT_PATH})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PD_OUTPUT_PATH})
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PD_OUTPUT_PATH})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

if(APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
elseif(WIN32 OR MINGW)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
else()
  if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_arm")
  elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_ia64")
  elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i386")
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_i386")
  else()
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
  endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "ossia")
