cmake_minimum_required(VERSION 3.0)
project(ossia-max C CXX)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INCLUDE_CURRENT_DIR 1)
include(GenerateExportHeader)
if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path/../../../../support;@loader_path")
elseif(WIN32)
    set(CMAKE_INSTALL_RPATH  "\$ORIGIN/../support;\$ORIGIN")
endif()
find_package(MaxSDK REQUIRED)

if(APPLE)
    FIND_PATH(FILE_H_DIR Files.h
        HINTS
        /System/Library/Frameworks/
        /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers/
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Headers)
    message( STATUS " Files.h have been found here : ${FILE_H_DIR}")
    if( ${FILE_H_DIR} EQUAL "FILE_H_DIR-NOT_FOUND")
        message( STATUS "  Files.h NOT FOUND !!" )
    else()
        include_directories("${FILE_H_DIR}")
    endif()
endif()

set(PROJECT_SRCS
    src/ossia-max.cpp
    src/ossia-max.hpp
    src/object_base.cpp
    src/object_base.hpp
    src/ossia_object.cpp
    src/ossia_object.hpp
    src/client.hpp
    src/client.cpp
    src/device.hpp
    src/device.cpp
    src/device_base.hpp
    src/device_base.cpp
    src/logger.hpp
    src/logger.cpp
    src/model.hpp
    src/model.cpp
    src/node_base.cpp
    src/node_base.hpp
    src/parameter.hpp
    src/parameter.cpp
    src/parameter_base.hpp
    src/parameter_base.cpp
    src/remote.hpp
    src/remote.cpp
    src/utils.hpp
    src/utils.cpp
    src/view.hpp
    src/view.cpp
    ${MAXSDK_MAINPATH}/c74support/max-includes/common/commonsyms.c
    )

add_library(${PROJECT_NAME} SHARED ${PROJECT_SRCS})
target_compile_options(${PROJECT_NAME} PRIVATE ${OSSIA_COMPILE_OPTIONS})
# add_custom_target just to display sources files in qtcreator
add_custom_target(source SOURCES ${PROJECT_SRCS})
use_gold(${PROJECT_NAME})
add_linker_warnings_external(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ossia ${MAXSDK_API_LIBRARY})

generate_export_header(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MAXSDK_MAX_INCLUDE_DIR})

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MAC_VERSION)
    target_include_directories(${PROJECT_NAME} PRIVATE ${FILE_H_DIR})
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++1z>)
elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN_VERSION _CRT_SECURE_NO_WARNINGS)
    if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll64")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".dll")
    endif()
endif()

function(setup_max_external theTarget)
    if(APPLE)
        set_property(TARGET ${theTarget} PROPERTY BUNDLE True)
        set_property(TARGET ${theTarget} PROPERTY BUNDLE_EXTENSION "mxo")
        file(COPY resources/PkgInfo DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${theTarget}.mxo/Contents/)
    elseif(WIN32)
        if("${CMAKE_SIZEOF_VOID_P}" MATCHES "8")
            set_target_properties(${theTarget} PROPERTIES SUFFIX ".mxe64")
        else()
            set_target_properties(${theTarget} PROPERTIES SUFFIX ".mxe")
        endif()
    endif()
endfunction()

# Shim externals to allow objects to be found by Max search box
set(OSSIA_EXT_LIST ossia.parameter ossia.model ossia.view ossia.remote ossia.device ossia.client ossia.logger ossia.ossia)
foreach(ossiaExt ${OSSIA_EXT_LIST})
    string(REPLACE "." "_" ossia_lib_name ${ossiaExt})
    configure_file(src/empty_external.c.in ${CMAKE_CURRENT_BINARY_DIR}/${ossiaExt}.c)
    add_library(${ossiaExt} MODULE ${CMAKE_CURRENT_BINARY_DIR}/${ossiaExt}.c)
    target_link_libraries(${ossiaExt} PRIVATE ${PROJECT_NAME})
    target_include_directories(${ossiaExt} PRIVATE ${CMAKE_BINARY_DIR}/OSSIA)
    setup_max_external(${ossiaExt})
endforeach()

set_target_properties(ossia.ossia PROPERTIES OUTPUT_NAME "ossia")

set(OSSIA_MAX_INSTALL_FOLDER "ossia-max-package/ossia" CACHE STRING "Max Package Install folder")

install(
    DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/help"
        "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        "${CMAKE_CURRENT_SOURCE_DIR}/init"
        "${CMAKE_CURRENT_SOURCE_DIR}/interfaces"
        "${CMAKE_CURRENT_SOURCE_DIR}/docs"
        "${CMAKE_CURRENT_SOURCE_DIR}/patchers"
    DESTINATION
        ${OSSIA_MAX_INSTALL_FOLDER}
    COMPONENT
        ${PROJECT_NAME}
)

install(
    TARGETS
        ${PROJECT_NAME}
    BUNDLE DESTINATION
        "${OSSIA_MAX_INSTALL_FOLDER}/support"
    RUNTIME DESTINATION
        "${OSSIA_MAX_INSTALL_FOLDER}/support"
    LIBRARY DESTINATION
        "${OSSIA_MAX_INSTALL_FOLDER}/support"
    COMPONENT
        ${PROJECT_NAME}
)

install(
    TARGETS
        ${OSSIA_EXT_LIST}
    BUNDLE DESTINATION
        "${OSSIA_MAX_INSTALL_FOLDER}/externals"
    RUNTIME DESTINATION
        "${OSSIA_MAX_INSTALL_FOLDER}/externals"
    LIBRARY DESTINATION
        "${OSSIA_MAX_INSTALL_FOLDER}/externals"
    COMPONENT
        ${PROJECT_NAME}
)
