cmake_minimum_required(VERSION 3.0)
project("ossia-pd")

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)

if(MSVC AND NOT EXISTS "${CMAKE_BINARY_DIR}/pd.lib")
    # On windows we have to create a .lib file for Pd since the original one is incomplete
    
    # First download Pd.
    file(DOWNLOAD "http://msp.ucsd.edu/Software/pd-0.47-1.msw.zip" "${CMAKE_BINARY_DIR}/pd.zip")
    
    # Extract Pd.
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_BINARY_DIR}/pd.zip"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/")
        
    # Then we follow this process :
    # https://adrianhenke.wordpress.com/2008/12/05/create-lib-file-from-dll
    execute_process(
        COMMAND dumpbin /exports "${CMAKE_BINARY_DIR}/pd/bin/pd.dll"
        OUTPUT_VARIABLE PD_WIN32_DEF)
        
    # Convert to list
    string(REPLACE "\n" ";" PD_WIN32_DEF "${PD_WIN32_DEF}")
    
    # Only keep the relevant lines
    list(FILTER PD_WIN32_DEF INCLUDE REGEX " *([0-9]+) *([A-F0-9]+) ([A-F0-9]+) (.*)")
    
    file(WRITE "${CMAKE_BINARY_DIR}\\pd.def" "EXPORTS\n")
    
    # Filter each line to get only the function name
    foreach(PD_FUN_LINE ${PD_WIN32_DEF})
        string(REGEX REPLACE " *([0-9]+) *([A-F0-9]+) ([A-F0-9]+) " ""  PD_FUN_LINE "${PD_FUN_LINE}")
        string(STRIP "${PD_FUN_LINE}" PD_FUN_LINE)
        file(APPEND "${CMAKE_BINARY_DIR}\\pd.def" "${PD_FUN_LINE}\n")
    endforeach()
    
    # Finally, create a .lib from the .def
    if(${CMAKE_GENERATOR} MATCHES "64")
        set(PD_LIB_MACHINE X64)
    else()
        set(PD_LIB_MACHINE X86)
    endif()
        execute_process(
            COMMAND lib "/MACHINE:${PD_LIB_MACHINE}" "/def:${CMAKE_BINARY_DIR}\\pd.def" "/OUT:${CMAKE_BINARY_DIR}/pd.lib")
endif()

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
if(MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/pd.lib")
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-undefined,dynamic_lookup)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,--warn-unresolved-symbols)
endif()

if(UNIX)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14)
endif()

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
