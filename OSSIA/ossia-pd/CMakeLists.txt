cmake_minimum_required(VERSION 3.0)
project("ossia-pd")

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)

set(OSSIA_PD_PATH "${OSSIA_3RDPARTY_FOLDER}/pure-data")
set(OSSIA_CICM_PATH "${OSSIA_3RDPARTY_FOLDER}/CicmWrapper")
include_directories("${OSSIA_PD_PATH}/src")

set(OLD_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("${OSSIA_CICM_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/Cicm-build")
set(BUILD_SHARED_LIBS ${OLD_BUILD_SHARED_LIBS})

include_directories("${OSSIA_CICM_PATH}/Sources")

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
use_gold(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE CicmWrapper ossia)
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-undefined,dynamic_lookup)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,--warn-unresolved-symbols)
endif()
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14)

# Win32 uses DLL as a suffix for pd externals
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ossia")

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
elseif(UNIX)
    if(${OSSIA_ARCHITECTURE} MATCHES "arm")
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_arm")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
    endif()
endif()
