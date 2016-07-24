cmake_minimum_required(VERSION 2.8)

project(APIJamomaImplementation CXX)
include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(APPLE)
  set(CMAKE_INSTALL_NAME_DIR @rpath)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks;@loader_path/../Frameworks/jamoma;@loader_path/../Frameworks/jamoma/extensions;@executable_path;/usr/local/jamoma/lib;")
endif()
set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)

find_package(Boost)

function(target_transfer_options TheTarget TheLib)
    target_include_directories(${TheTarget} PUBLIC
        $<TARGET_PROPERTY:${TheLib},INTERFACE_INCLUDE_DIRECTORIES>
        )
    target_compile_definitions(${TheTarget} PUBLIC
        $<TARGET_PROPERTY:${TheLib},INTERFACE_COMPILE_DEFINITIONS>
        )
    target_compile_options(${TheTarget} PUBLIC
        $<TARGET_PROPERTY:${TheLib},INTERFACE_COMPILE_OPTIONS>
        )
endfunction()

include_directories(${OSSIA_HEADERS_FOLDER})
include_directories(${OSSIA_HEADERS_FOLDER}/Editor)
include_directories(${OSSIA_HEADERS_FOLDER}/Misc)
include_directories(${OSSIA_HEADERS_FOLDER}/Network)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Includes)

file(GLOB_RECURSE NETWORK_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Network/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Network/*.cpp"
)

find_package(Jamoma 0.6)
if(NOT ${Jamoma_FOUND})
    message("Jamoma not found - won't build Network API")
    return()
endif()

add_library(APIJamoma ${NETWORK_SRCS})
if(WIN32)
    target_compile_definitions(APIJamoma PUBLIC
        TT_PLATFORM_WIN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS)
    if(MSVC)
        target_compile_options(APIJamoma PUBLIC
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        )
    endif()
else()
    target_compile_options(APIJamoma PRIVATE ${OSSIA_COMPILE_FLAGS})
endif()

set_target_properties(APIJamoma PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	VISIBILITY_INLINES_HIDDEN 0)

target_link_libraries(APIJamoma PUBLIC
    OSSIA_common
    Jamoma::Foundation
    Jamoma::Modular
    ModernMIDI)

generate_export_header(APIJamoma BASE_NAME OSSIA)
target_include_directories(APIJamoma PUBLIC
    "${OSSIA_HEADERS_FOLDER}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}" )


if(OSSIA_SANITIZE)
    sanitize_build(APIJamoma)
    #debugmode_build(APIJamoma)
else()
    use_gold(APIJamoma)
endif()
add_linker_warnings(APIJamoma)

target_compile_definitions(APIJamoma PRIVATE OSSIA_BOOST)
