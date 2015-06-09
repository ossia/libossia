cmake_minimum_required(VERSION 2.8)

project(APIJamomaImplementation)

find_package(Jamoma 0.6)
if(NOT ${Jamoma_FOUND})
    message("Jamoma not found - won't build Jamoma API")
    return()
endif()

include_directories(${API_HEADERS_FOLDER}/Editor)
include_directories(${API_HEADERS_FOLDER}/Misc)
include_directories(${API_HEADERS_FOLDER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Includes)

file(GLOB_RECURSE SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Network/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Network/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Editor/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Editor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Misc/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Misc/*.cpp")

add_library(APIJamoma ${SRCS})
target_link_libraries(APIJamoma  Jamoma::Foundation Jamoma::Modular Jamoma::Score)
