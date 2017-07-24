cmake_minimum_required(VERSION 3.0)
project(ossia-max C CXX)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)

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
    src/ossia_object_base.cpp
    src/ossia_object_base.hpp
    src/ossia_object.cpp
    src/ossia_object.hpp
    src/parameter.hpp
    src/parameter.cpp
    src/client.hpp
    src/client.cpp
    src/device.hpp
    src/device.cpp
    src/model.hpp
    src/model.cpp
    src/logger.hpp
    src/logger.cpp
    src/remote.hpp
    src/remote.cpp
    src/utils.hpp
    src/utils.cpp
    src/view.hpp
    src/view.cpp
    ${MAXSDK_MAINPATH}/c74support/max-includes/common/commonsyms.c
    )

add_library(${PROJECT_NAME} MODULE ${PROJECT_SRCS})
use_gold(${PROJECT_NAME})
add_linker_warnings_external(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ossia ${MAXSDK_API_LIBRARY})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MAXSDK_MAX_INCLUDE_DIR})

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE MAC_VERSION)
    target_include_directories(${PROJECT_NAME} PRIVATE ${FILE_H_DIR})
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++1z>)

    set_property(TARGET ${PROJECT_NAME} PROPERTY BUNDLE True)
    set_property(TARGET ${PROJECT_NAME} PROPERTY BUNDLE_EXTENSION "mxo")
    file(COPY resources/PkgInfo DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/ossia-max.mxo/Contents/)
elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN_VERSION _CRT_SECURE_NO_WARNINGS)
    if(WIN64)
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".mxe64")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".mxe")
    endif()
endif()

set(OSSIA_MAX_INSTALL_FOLDER "ossia-max-package/ossia" CACHE STRING "Max Package Install folder")

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/help"
        DESTINATION ${OSSIA_MAX_INSTALL_FOLDER}
        COMPONENT ${PROJECT_NAME})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/examples"
        DESTINATION ${OSSIA_MAX_INSTALL_FOLDER}
        COMPONENT ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
        BUNDLE RUNTIME LIBRARY
        DESTINATION "${OSSIA_MAX_INSTALL_FOLDER}"
        COMPONENT ${PROJECT_NAME})
