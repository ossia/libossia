cmake_minimum_required(VERSION 2.8)

project(APIJamomaImplementation)

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@loader_path/../Frameworks/jamoma;@loader_path/../Frameworks/jamoma/extensions;@executable_path;/usr/local/jamoma/lib;")
endif()

include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)

function(target_transfer_options TheTarget TheLib)
    target_include_directories(${TheTarget} PUBLIC
        $<TARGET_PROPERTY:${TheLib},INTERFACE_INCLUDE_DIRECTORIES>
        )
    target_compile_definitions(${TheTarget} PRIVATE
        $<TARGET_PROPERTY:${TheLib},INTERFACE_COMPILE_DEFINITIONS>
        )
    target_compile_options(${TheTarget} PRIVATE
        $<TARGET_PROPERTY:${TheLib},INTERFACE_COMPILE_OPTIONS>
        )
endfunction()

include_directories(${API_HEADERS_FOLDER}/Editor)
include_directories(${API_HEADERS_FOLDER}/Misc)
include_directories(${API_HEADERS_FOLDER}/Network)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Includes)

file(GLOB_RECURSE MISC_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Misc/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Misc/*.cpp"
)

file(GLOB_RECURSE EDITOR_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Editor/*.cpp"
)

# We make OBJECT libraries to reuse the object files
# for all the targets
add_library(APIJamoma_Misc_Object   OBJECT ${MISC_SRCS})
add_library(APIJamoma_Editor_Object OBJECT ${EDITOR_SRCS})

add_library(APIJamoma_Editor
    $<TARGET_OBJECTS:APIJamoma_Misc_Object>
    $<TARGET_OBJECTS:APIJamoma_Editor_Object>)

# We can build up to this point without Jamoma

find_package(Jamoma 0.6)
if(NOT ${Jamoma_FOUND})
    message("Jamoma not found - won't build Network API")
    return()
endif()

file(GLOB_RECURSE NETWORK_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Network/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Network/*.cpp"
)

add_library(APIJamoma_Network_Object OBJECT ${NETWORK_SRCS})
target_transfer_options(APIJamoma_Network_Object Jamoma::Foundation)
target_transfer_options(APIJamoma_Network_Object Jamoma::Modular)

add_library(APIJamoma
    $<TARGET_OBJECTS:APIJamoma_Misc_Object>
    $<TARGET_OBJECTS:APIJamoma_Network_Object>
    $<TARGET_OBJECTS:APIJamoma_Editor_Object>)

target_link_libraries(APIJamoma PRIVATE Jamoma::Foundation Jamoma::Modular)
target_include_directories(APIJamoma PUBLIC "${API_HEADERS_FOLDER}")

if(OSSIA_SANITIZE)
    sanitize_build(APIJamoma)
    #debugmode_build(APIJamoma)
endif()
use_gold(APIJamoma)
add_linker_warnings(APIJamoma)
