# - Try to find Portaudio
# Once done this will define
#
#  PORTAUDIO_FOUND - system has Portaudio
#  PORTAUDIO_INCLUDE_DIRS - the Portaudio include directory
#  PORTAUDIO_LIBRARIES - Link these to use Portaudio
#  PORTAUDIO_DEFINITIONS - Compiler switches required for using Portaudio
#  PORTAUDIO_VERSION - Portaudio version
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
# Redistribution and use is allowed according to the terms of the New BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if(NOT PORTAUDIO2_FOUND OR "${PORTAUDIO_INCLUDE_DIRS}" MATCHES "")
    find_path(PORTAUDIO_INCLUDE_DIRS
        NAMES
        portaudio.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        c:/portaudio/include
        "${PORTAUDIO_INCLUDE_DIR_HINT}"
        )

    find_library(PORTAUDIO_LIBRARIES
        NAMES
        portaudio_static_x64.lib libportaudio_static.a libportaudio.a portaudio
        PATHS
        /opt/portaudio/lib
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        c:/portaudio/lib
        "${PORTAUDIO_LIB_DIR_HINT}"
        )

    if(${PORTAUDIO_LIBRARIES} MATCHES ".*\.a")
        add_library(PortAudio STATIC IMPORTED)
    elseif(${PORTAUDIO_LIBRARIES} MATCHES ".*portaudio_static_x64\.lib")
        add_library(PortAudio STATIC IMPORTED)
    elseif(${PORTAUDIO_LIBRARIES} MATCHES ".*{lib,so,dylib,dll}")
        add_library(PortAudio SHARED IMPORTED)
    else()
        return()
    endif()

    set_target_properties(PortAudio
        PROPERTIES
          IMPORTED_LOCATION ${PORTAUDIO_LIBRARIES}
          INTERFACE_INCLUDE_DIRECTORIES ${PORTAUDIO_INCLUDE_DIRS})
    set(PORTAUDIO_VERSION 19)
endif ()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Portaudio DEFAULT_MSG
    PORTAUDIO_LIBRARIES PORTAUDIO_INCLUDE_DIRS)
