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

  if(OSSIA_CI)
    set(PORTAUDIO_ONLY_DYNAMIC 1)
  endif()

  if(WIN32)
    find_library(PORTAUDIO_LIBRARIES_DEBUG
      NAMES
        portaudio_static_x64.lib
      PATHS
        c:/portaudio/lib/debug
        "${PORTAUDIO_LIB_DIR_HINT}/debug"
    )
    find_library(PORTAUDIO_LIBRARIES_RELEASE
      NAMES
        portaudio_static_x64.lib
      PATHS
        c:/portaudio/lib/release
        "${PORTAUDIO_LIB_DIR_HINT}/release"
    )

    set(PORTAUDIO_LIBRARIES)
    if(PORTAUDIO_LIBRARIES_DEBUG)
      set(PORTAUDIO_LIBRARIES ${PORTAUDIO_LIBRARIES} ${PORTAUDIO_LIBRARIES_DEBUG})
    endif()
    if(PORTAUDIO_LIBRARIES_RELEASE)
      set(PORTAUDIO_LIBRARIES ${PORTAUDIO_LIBRARIES} ${PORTAUDIO_LIBRARIES_RELEASE})
    endif()

    if(NOT PORTAUDIO_LIBRARIES_DEBUG AND NOT PORTAUDIO_LIBRARIES_RELEASE)
      return()
    endif()

    add_library(PortAudio STATIC IMPORTED)

    set_property(
      TARGET PortAudio
      APPEND
      PROPERTY
        IMPORTED_CONFIGURATIONS DEBUG RELEASE
    )
    set_target_properties(PortAudio
      PROPERTIES
      IMPORTED_LOCATION_DEBUG ${PORTAUDIO_LIBRARIES_DEBUG}
      IMPORTED_LOCATION_RELEASE ${PORTAUDIO_LIBRARIES_RELEASE}
      IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
      IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
      MAP_IMPORTED_CONFIG_MINSIZEREL Release
      MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
      INTERFACE_INCLUDE_DIRECTORIES ${PORTAUDIO_INCLUDE_DIRS})
    set(PORTAUDIO_VERSION 19)
  else()

    if(PORTAUDIO_ONLY_DYNAMIC)
      find_library(PORTAUDIO_LIBRARIES
        NAMES
          portaudio
        PATHS
          /opt/portaudio/lib
          /usr/lib
          /usr/local/lib
          /opt/local/lib
          c:/portaudio/lib
          "${PORTAUDIO_LIB_DIR_HINT}"
      )
    else()
        find_library(PORTAUDIO_LIBRARIES
           NAMES
             portaudio_static_x64.lib libportaudio_static.a libportaudio.a portaudio
           PATHS
             /opt/portaudio/lib
             /usr/lib
             /usr/local/lib
             /opt/local/lib
             "${PORTAUDIO_LIB_DIR_HINT}"
          )
    endif()

    if("${PORTAUDIO_LIBRARIES}" MATCHES ".*\.a")
      add_library(PortAudio STATIC IMPORTED)

      include(CheckLibraryExists)
      check_library_exists(${PORTAUDIO_LIBRARIES} PaJack_GetClientName "" PORTAUDIO_NEEDS_JACK)
      if(PORTAUDIO_NEEDS_JACK)
        find_library(Jack_LIBRARY NAMES jack)
        if(Jack_LIBRARY)
          set_target_properties(PortAudio
              PROPERTIES
              INTERFACE_LINK_LIBRARIES ${Jack_LIBRARY}
          )
        else()
          message(FATAL_ERROR "Static portaudio lib links against Jack but Jack wasn't found")
        endif()
      endif()
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
  endif()
endif ()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  Portaudio DEFAULT_MSG
  PORTAUDIO_LIBRARIES PORTAUDIO_INCLUDE_DIRS)
