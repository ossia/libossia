if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(zita-alsa-pcmi CONFIG GLOBAL)
endif()

if(NOT TARGET zita-alsa-pcmi)
  add_library(zita-alsa-pcmi STATIC
      "${OSSIA_3RDPARTY_FOLDER}/zita-alsa-pcmi/zita-alsa-pcmi-ardour.cc"
      "${OSSIA_3RDPARTY_FOLDER}/zita-alsa-pcmi/zita-alsa-pcmi-ardour.h"
  )
  target_include_directories(zita-alsa-pcmi
      PRIVATE "${OSSIA_SOURCE_FOLDER}/src"
      PUBLIC "${OSSIA_3RDPARTY_FOLDER}/zita-alsa-pcmi"
  )
endif()

