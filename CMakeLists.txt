cmake_minimum_required(VERSION 2.8)
project(API)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/CMake")

option(COVERALLS "Generate coveralls data" OFF)

if (COVERALLS)
    include(Coveralls)
    coveralls_turn_on_coverage()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(API_HEADERS_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Headers")
set(API_IMPL_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Implementations")

file(GLOB_RECURSE HEADERS Headers/*.h Toolkit/*.h)
add_custom_target(OSSIAHeaders SOURCES ${HEADERS})
add_library(OSSIAHeadersLib INTERFACE)

target_include_directories(OSSIAHeadersLib INTERFACE "${API_HEADERS_FOLDER}")
include_directories(Headers)

add_subdirectory(Implementations)
#if(NOT OSSIA_NO_EXAMPLES)
    add_subdirectory(Documentation/Examples)
#endif()

if(NOT OSSIA_NO_TESTS)
    add_subdirectory(Tests)
endif()
