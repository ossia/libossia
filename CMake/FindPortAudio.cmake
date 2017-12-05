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
    find_path(PORTAUDIO_INCLUDE_DIR
        NAMES
        portaudio.h
        PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        "${PORTAUDIO_INCLUDE_DIR_HINT}"
        )

    find_library(PORTAUDIO_LIBRARY
        NAMES
        libportaudio_static.a libportaudio_static.lib libportaudio.a portaudio
        PATHS
        /opt/portaudio/lib
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        "${PORTAUDIO_LIB_DIR_HINT}"
        )

    if(${PORTAUDIO_LIBRARY} MATCHES ".*\.a")
        add_library(PortAudio STATIC IMPORTED)
    elseif(${PORTAUDIO_LIBRARY} MATCHES ".*libportaudio_static\.lib")
        add_library(PortAudio STATIC IMPORTED)
    elseif(${PORTAUDIO_LIBRARY} MATCHES ".*{lib,so,dylib,dll}")
        add_library(PortAudio SHARED IMPORTED)
    else()
        return()
    endif()

    set_target_properties(PortAudio
        PROPERTIES
          IMPORTED_LOCATION ${PORTAUDIO_LIBRARY}
          INTERFACE_INCLUDE_DIRECTORIES ${PORTAUDIO_INCLUDE_DIR})
    set(PORTAUDIO_VERSION 19)
endif ()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Portaudio DEFAULT_MSG
    PORTAUDIO_LIBRARIES PORTAUDIO_INCLUDE_DIRS)
