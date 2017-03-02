cmake_minimum_required(VERSION 3.0)
project("ossia-max")

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_MACOSX_RPATH Off)

find_package(MaxSDK REQUIRED)

if ( APPLE )
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
include_directories(src)
include_directories(${MAXSDK_MAX_INCLUDE_DIR})

set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty")
include_directories(SYSTEM "${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include")

set(PROJECT_SRCS
    src/ossia-max.cpp
    src/ossia-max.hpp
    )

add_library(${PROJECT_NAME} MODULE ${PROJECT_SRCS})
use_gold(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ossia ${MAXSDK_API_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE ${FILE_H_DIR})
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,-undefined,dynamic_lookup)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE -Wl,--warn-unresolved-symbols)
endif()

if(UNIX)
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++14)
endif()

if(APPLE)
    set_property(TARGET ${PROJECT_NAME} PROPERTY BUNDLE True)
    set_property(TARGET ${PROJECT_NAME} PROPERTY BUNDLE_EXTENSION "mxo")
elseif(WIN32)
    if(WIN64)
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".mxe64")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".mxe")
    endif()
endif()

