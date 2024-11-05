if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_path(ZITA_ALSA_PCMI_INCLUDE_DIR zita-alsa-pcmi.h)
  find_library(ZITA_ALSA_PCMI_LIBRARY zita-alsa-pcmi)

  if(ZITA_ALSA_PCMI_INCLUDE_DIR AND ZITA_ALSA_PCMI_LIBRARY)
    add_library(zita-alsa-pcmi INTERFACE)
    target_include_directories(zita-alsa-pcmi INTERFACE ${ZITA_ALSA_PCMI_INCLUDE_DIR})
    target_link_libraries(zita-alsa-pcmi INTERFACE ${ZITA_ALSA_PCMI_LIBRARY})
  endif()
endif()

if(NOT TARGET zita-alsa-pcmi)
  find_path(ASOUNDLIB_INCLUDE_DIRS alsa/asoundlib.h)
  if(ASOUNDLIB_INCLUDE_DIRS)
    add_library(zita-alsa-pcmi STATIC
        "${OSSIA_3RDPARTY_FOLDER}/zita-alsa-pcmi/zita-alsa-pcmi-ardour.cc"
        "${OSSIA_3RDPARTY_FOLDER}/zita-alsa-pcmi/zita-alsa-pcmi-ardour.h"
    )
    target_include_directories(zita-alsa-pcmi
        PRIVATE "${OSSIA_SOURCE_FOLDER}/src"
        PUBLIC
          "${OSSIA_3RDPARTY_FOLDER}/zita-alsa-pcmi"
          "${ASOUNDLIB_INCLUDE_DIRS}"
    )
  endif()
endif()

