cmake_minimum_required(VERSION 2.8)

project(APIJamomaImplementation)
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
include_directories(${OSSIA_3RDPARTY_FOLDER}/variant/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Includes)

file(GLOB_RECURSE MISC_SRCS
    "${OSSIA_HEADERS_FOLDER}/Misc/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Misc/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Misc/*.cpp"
)

file(GLOB_RECURSE EDITOR_SRCS
    "${OSSIA_HEADERS_FOLDER}/Editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Editor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Editor/*.cpp"
)

file(GLOB_RECURSE NETWORK_SRCS
    "${OSSIA_HEADERS_FOLDER}/Network/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Includes/Network/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Network/*.cpp"
)

# We make OBJECT libraries to reuse the object files
# for all the targets.
# Not in win32 due to https://gitlab.kitware.com/cmake/cmake/issues/15857
if(WIN32)
    find_package(Jamoma 0.6)
    if(NOT ${Jamoma_FOUND})
        message("Jamoma not found - won't build Network API")
        return()
    endif()

    add_library(APIJamoma
        ${MISC_SRCS}
        ${EDITOR_SRCS}
        ${NETWORK_SRCS})
    target_compile_definitions(APIJamoma PUBLIC
        TT_PLATFORM_WIN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS)
    target_link_libraries(APIJamoma PUBLIC
        Jamoma::Foundation
        Jamoma::Modular
        ModernMIDI)
    if(MSVC)
        target_compile_options(APIJamoma PUBLIC
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        )
    endif()

    if(Boost_FOUND)
        # Allows the use of faster containers
        target_compile_definitions(APIJamoma PUBLIC OSSIA_BOOST)
        target_include_directories(APIJamoma PUBLIC ${Boost_INCLUDE_DIRS})
    endif()
else()    
    set(OSSIA_COMPILE_FLAGS
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-unknown-pragmas
        -Wnon-virtual-dtor
        -pedantic
        -Woverloaded-virtual
        -pipe
        -Werror=return-type
        -Werror=trigraphs)

    add_library(APIJamoma_Misc_Object   OBJECT ${MISC_SRCS})
    add_library(APIJamoma_Editor_Object OBJECT ${EDITOR_SRCS})

    add_library(APIJamoma_Editor
        $<TARGET_OBJECTS:APIJamoma_Misc_Object>
        $<TARGET_OBJECTS:APIJamoma_Editor_Object>)

    if(Boost_FOUND)
        # Allows the use of faster containers
        target_compile_definitions(APIJamoma_Editor PUBLIC OSSIA_BOOST)
        target_include_directories(APIJamoma_Editor PUBLIC ${Boost_INCLUDE_DIRS})
    endif()

    # We can build up to this point without Jamoma

    find_package(Jamoma 0.6)
    if(NOT ${Jamoma_FOUND})
        message("Jamoma not found - won't build Network API")
        return()
    endif()


    add_library(APIJamoma_Network_Object OBJECT ${NETWORK_SRCS})
    target_transfer_options(APIJamoma_Network_Object Jamoma::Foundation)
    target_transfer_options(APIJamoma_Network_Object Jamoma::Modular)
    target_transfer_options(APIJamoma_Network_Object ModernMIDI)

    target_compile_options(APIJamoma_Misc_Object PRIVATE ${OSSIA_COMPILE_FLAGS})
    target_compile_options(APIJamoma_Network_Object PRIVATE ${OSSIA_COMPILE_FLAGS})
    target_compile_options(APIJamoma_Editor_Object PRIVATE ${OSSIA_COMPILE_FLAGS})

    add_library(APIJamoma
        $<TARGET_OBJECTS:APIJamoma_Misc_Object>
        $<TARGET_OBJECTS:APIJamoma_Network_Object>
        $<TARGET_OBJECTS:APIJamoma_Editor_Object>)

    target_compile_options(APIJamoma PRIVATE ${OSSIA_COMPILE_FLAGS})

    target_link_libraries(APIJamoma PRIVATE Jamoma::Foundation Jamoma::Modular ModernMIDI)
endif()
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
