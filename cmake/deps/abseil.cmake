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

    # Abseil's CMakeLists forcibly overrides CMAKE_MSVC_RUNTIME_LIBRARY from its
    # own ABSL_MSVC_STATIC_RUNTIME option (default OFF -> /MD), ignoring what the
    # rest of the build selected. Static ossia builds (Max/PD/Python/Unity, ...)
    # use the static /MT runtime, which then mismatches Abseil's /MD at link
    # time. Mirror ossia's choice so the runtimes agree.
    if(MSVC AND CMAKE_MSVC_RUNTIME_LIBRARY AND NOT CMAKE_MSVC_RUNTIME_LIBRARY MATCHES "DLL")
      set(ABSL_MSVC_STATIC_RUNTIME ON CACHE INTERNAL "")
    else()
      set(ABSL_MSVC_STATIC_RUNTIME OFF CACHE INTERNAL "")
    endif()

    # When we install our static dependencies, re2 links Abseil publicly, so its
    # targets must be installable/exportable too.
    if(OSSIA_INSTALL_STATIC_DEPENDENCIES)
      set(ABSL_ENABLE_INSTALL ON CACHE INTERNAL "")

      # NB: no EXCLUDE_FROM_ALL here. CMake skips the install() rules of a
      # subdirectory added with EXCLUDE_FROM_ALL, so Abseil's libraries,
      # headers and CMake package (abslConfig / abslTargets) would never be
      # installed - leaving the installed re2 export (which links Abseil
      # publicly) dangling for downstream consumers. Building all of Abseil is
      # acceptable here as this branch is only taken for static packaging builds.
      add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/abseil-cpp")
    else()
      add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/abseil-cpp" EXCLUDE_FROM_ALL)
    endif()
  endblock()
endif()

if(TARGET absl::strings)
  set(OSSIA_HAS_ABSEIL 1 CACHE INTERNAL "")
endif()
