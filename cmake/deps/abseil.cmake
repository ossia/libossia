if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(absl CONFIG GLOBAL QUIET)
endif()

# Abseil is pulled in as a dependency of re2 (see deps/re2.cmake).
# We build the in-tree copy through its own CMake build, like the other
# add_subdirectory-based dependencies (rubberband, libremidi, ...).
if(NOT TARGET absl::strings)
  block()
    set(BUILD_SHARED_LIBS 0)
    set(BUILD_TESTING 0)
    set(ABSL_BUILD_TESTING OFF CACHE INTERNAL "")
    set(ABSL_PROPAGATE_CXX_STD ON CACHE INTERNAL "")
    # Treat Abseil's headers as system headers so that re2 (and ossia) do not
    # get flooded with warnings coming from them.
    set(ABSL_USE_SYSTEM_INCLUDES ON CACHE INTERNAL "")

    # When we install our static dependencies, re2 links Abseil publicly, so its
    # targets must be installable/exportable too.
    if(OSSIA_INSTALL_STATIC_DEPENDENCIES)
      set(ABSL_ENABLE_INSTALL ON CACHE INTERNAL "")
    endif()

    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/abseil-cpp" EXCLUDE_FROM_ALL)
  endblock()
endif()

if(TARGET absl::strings)
  set(OSSIA_HAS_ABSEIL 1 CACHE INTERNAL "")
endif()
