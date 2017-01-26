set(CMAKE_MODULE_PATH
  "${CMAKE_MODULE_PATH}"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

cmake_minimum_required(VERSION 3.5)
project(OSSIA)

# Configuration
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)

include(OssiaConfiguration)

# Dependencies
set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")

find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

if(NOT CMAKE_SYSTEM_NAME MATCHES Emscripten)
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/ModernMIDI" EXCLUDE_FROM_ALL)
endif()

add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)

# Main library
add_subdirectory(OSSIA)

if(OSSIA_PD)
    add_subdirectory(OSSIA/ossia-pd)
endif()

if(OSSIA_PYTHON)
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/pybind11")
    pybind11_add_module(ossia_python OSSIA/ossia-python/ossia_python.cpp)
    target_link_libraries(ossia_python ossia)
endif()
# Examples & tests
if(OSSIA_EXAMPLES)
    add_subdirectory(Documentation/Examples)
endif()

if(OSSIA_TESTING)
    enable_testing()
    add_subdirectory(Tests)
endif()
