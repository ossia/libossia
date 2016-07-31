cmake_minimum_required(VERSION 2.8)
project(API)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/CMake;${PROJECT_SOURCE_DIR}/CMake/cmake-modules;")

option(OSSIA_COVERALLS "Generate coveralls data" OFF)
option(OSSIA_SANITIZE "Sanitize build" OFF) # TODO Jamoma needs to be compiled with this flag too.

if (OSSIA_COVERALLS)
    include(Coveralls)
    coveralls_turn_on_coverage()
endif()

set(CMAKE_CXX_STANDARD 14)

set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

add_subdirectory(3rdparty/ModernMIDI EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)
add_subdirectory(OSSIA)

if(NOT OSSIA_NO_EXAMPLES)
    add_subdirectory(Documentation/Examples)
endif()
set(OSSIA_NO_TESTS 0)
add_subdirectory(Tests/v2)
if(NOT OSSIA_NO_TESTS)
    add_subdirectory(Tests)
endif()
