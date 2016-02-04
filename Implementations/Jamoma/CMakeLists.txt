cmake_minimum_required(VERSION 2.8)

project(APIJamomaImplementation)

find_package(Jamoma 0.6)
if(NOT ${Jamoma_FOUND})
    message("Jamoma not found - won't build Jamoma API")
    return()
endif()
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)


include_directories(${API_HEADERS_FOLDER}/Editor)
include_directories(${API_HEADERS_FOLDER}/Misc)
include_directories(${API_HEADERS_FOLDER}/Network)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Includes)

file(GLOB_RECURSE SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Editor/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Misc/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Network/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Editor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Misc/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Network/*.cpp"
    )

add_library(APIJamoma ${SRCS})
target_link_libraries(APIJamoma PRIVATE Jamoma::Foundation Jamoma::Modular)
target_include_directories(APIJamoma PUBLIC "${API_HEADERS_FOLDER}")

if(OSSIA_SANITIZE)
    sanitize_build(APIJamoma)
    debugmode_build(APIJamoma)
endif()
use_gold(APIJamoma)
add_linker_warnings(APIJamoma)
