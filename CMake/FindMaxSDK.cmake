# https://github.com/marsyas/marsyas/blob/master/cmake-modules/FindMaxSDK.cmake

set(MAXSDK_MAX_INCLUDE_DIR NOTFOUND)
set(MAXSDK_MSP_INCLUDE_DIR NOTFOUND)
set(MAXSDK_API_LIBRARY NOTFOUND)
set(MAXSDK_AUDIO_LIBRARY NOTFOUND)
set(MAXSDK_CRT_LIBRARY NOTFOUND)

file(GLOB MAXSDK_DIRS "$ENV{HOME}/Documents/Max 7/Packages/max-sdk*/source")
find_path(
    MAXSDK_MAINPATH "c74support"
    HINTS "${MAXSDK_DIRS}"
    DOC "Insert main path to MaxSDK/source/c74support (download it from http://cycling74.com/products/sdk/ for both MacOSX and Windows)")

# Just to test if Max/MSP found!!
if(MAXSDK_MAINPATH)
    find_path(MAXSDK_MAX_INCLUDE_DIR MaxAPI.lib HINTS "${MAXSDK_MAINPATH}/c74support/max-includes/")
    if(MAXSDK_MAX_INCLUDE_DIR)
        set(MAXSDK_FOUND TRUE)
        message (STATUS "Max/MSP SDK Found at: ${MAXSDK_MAINPATH}")
    else()
        message (SEND_ERROR "Couldn't find: MAXSDK")
    endif()
endif()

if (MAXSDK_FOUND)
    find_path(MAXSDK_MAX_INCLUDE_DIR MaxAPI.lib HINTS "${MAXSDK_MAINPATH}/c74support/max-includes/")
    find_path(MAXSDK_MSP_INCLUDE_DIR MaxAudio.lib HINTS "${MAXSDK_MAINPATH}/c74support/msp-includes/")

    if(MAXSDK_MAX_INCLUDE_DIR AND MAXSDK_MSP_INCLUDE_DIR)
        message (STATUS "Found Max/MSP SDK Headers at: ${MAXSDK_MAX_INCLUDE_DIR}; ${MAXSDK_MSP_INCLUDE_DIR}")
    else()
        message (SEND_ERROR "${MAXSDK_MAX_INCLUDE_DIR}; ${MAXSDK_MSP_INCLUDE_DIR}")
    endif()

    if(APPLE)
        find_library(MAXSDK_API_LIBRARY NAMES MaxAPI HINTS "${MAXSDK_MAINPATH}/c74support/max-includes/")
        find_library(MAXSDK_AUDIO_LIBRARY NAMES MaxAudioAPI HINTS "${MAXSDK_MAINPATH}/c74support/msp-includes/")

        if(MAXSDK_API_LIBRARY AND MAXSDK_AUDIO_LIBRARY)
            message (STATUS "Found Max/MSP SDK Frameworks at: ${MAXSDK_API_LIBRARY}; ${MAXSDK_AUDIO_LIBRARY}")
        else()
            message (SEND_ERROR "${MAXSDK_API_LIBRARY}; ${MAXSDK_AUDIO_LIBRARY}")
        endif()
    elseif(WIN32)
        find_library(MAXSDK_API_LIBRARY NAMES MaxAPI.lib HINTS "${MAXSDK_MAINPATH}/c74support/max-includes/")
        find_library(MAXSDK_AUDIO_LIBRARY NAMES MaxAudio.lib HINTS "${MAXSDK_MAINPATH}/c74support/msp-includes/")
        find_library(MAXSDK_CRT_LIBRARY NAMES maxcrt.lib HINTS "${MAXSDK_MAINPATH}/c74support/max-includes/")

        if(MAXSDK_API_LIBRARY AND MAXSDK_AUDIO_LIBRARY AND MAXSDK_CRT_LIBRARY)
            message (STATUS "Found Max/MSP SDK Libraries at: ${MAXSDK_API_LIBRARY}; ${MAXSDK_AUDIO_LIBRARY}; ${MAXSDK_CRT_LIBRARY}")
        else()
            message (SEND_ERROR "${MAXSDK_API_LIBRARY}; ${MAXSDK_AUDIO_LIBRARY}; ${MAXSDK_CRT_LIBRARY}")
        endif()
    endif()
endif ()
