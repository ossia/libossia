if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(rnd CONFIG GLOBAL)
endif()

if(NOT TARGET rnd::rnd)
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/rnd")
endif()
