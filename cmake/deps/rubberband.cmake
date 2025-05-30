if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_library(RUBBERBAND_LIBRARY NAMES rubberband)
  find_path(RUBBERBAND_INCLUDE_DIR rubberband/RubberBandStretcher.h)

  if(RUBBERBAND_LIBRARY AND RUBBERBAND_INCLUDE_DIR)
    add_library(rubberband IMPORTED INTERFACE GLOBAL)
    target_include_directories(rubberband INTERFACE ${RUBBERBAND_INCLUDE_DIR})
    target_link_libraries(rubberband INTERFACE ${RUBBERBAND_LIBRARY})
  endif()
endif()

if(NOT TARGET rubberband)
  block()
    set(BUILD_SHARED_LIBS 0)
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/rubberband" EXCLUDE_FROM_ALL)
  endblock()
endif()
