cmake_minimum_required(VERSION 3.5)
project(OSSIA)

cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/CMake;${PROJECT_SOURCE_DIR}/CMake/cmake-modules;")

option(OSSIA_SANITIZE "Sanitize build" OFF)
option(OSSIA_TIDY "Use clang-tidy" OFF)


set(CMAKE_CXX_STANDARD 14)

set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

add_subdirectory(3rdparty/ModernMIDI EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)
add_subdirectory(OSSIA)

set(OSSIA_TESTS 1)
set(OSSIA_EXAMPLES 1)

if(OSSIA_EXAMPLES)
    add_subdirectory(Documentation/Examples)
endif()

if(OSSIA_TESTS)
    add_subdirectory(Tests)
    add_subdirectory(Tests/v2)
endif()
