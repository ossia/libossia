cmake_minimum_required(VERSION 2.8)

project(BasicOSSIA)
include(GenerateExportHeader)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)

find_package(Boost REQUIRED)

set(SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/BasicOSC.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/BasicOSC.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/BasicLocal.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/BasicLocal.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/BasicMinuit.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Network/BasicMinuit.cpp"
    )

add_library(BasicOSSIA ${SRCS})
if(WIN32)
    target_compile_definitions(BasicOSSIA PUBLIC
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS)
endif()

if(MSVC)
    target_compile_options(BasicOSSIA PUBLIC
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        )
endif()

target_include_directories(BasicOSSIA PUBLIC ${Boost_INCLUDE_DIRS})
target_compile_options(BasicOSSIA PRIVATE ${OSSIA_COMPILE_FLAGS})

generate_export_header(BasicOSSIA BASE_NAME OSSIA)
target_include_directories(BasicOSSIA PUBLIC
    "${OSSIA_HEADERS_FOLDER}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}" )


if(OSSIA_SANITIZE)
    sanitize_build(BasicOSSIA)
    #debugmode_build(APIJamoma)
else()
    use_gold(BasicOSSIA)
endif()
add_linker_warnings(BasicOSSIA)


