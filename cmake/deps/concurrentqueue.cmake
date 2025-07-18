if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(concurrentqueue 1.0 CONFIG GLOBAL)
  if(TARGET concurrentqueue::concurrentqueue)
    get_target_property(cq_include_dirs concurrentqueue::concurrentqueue  INTERFACE_INCLUDE_DIRECTORIES)

    # concurrentqueue package's cmake files as of 2025-07-18 still do not add the target_include_directories
    # concurrentqueue/moodycamel, leading to build issues
    foreach(cq_include_dir "${cq_include_dirs}")
      if("${cq_include_dir}" MATCHES ".*concurrentqueue.*")
        target_include_directories(concurrentqueue::concurrentqueue INTERFACE "$<BUILD_INTERFACE:${cq_include_dir}/moodycamel>")
        break()
      endif()
    endforeach()
  endif()
endif()

if(NOT TARGET concurrentqueue::concurrentqueue)
  add_library(concurrentqueue INTERFACE IMPORTED GLOBAL)
  add_library(concurrentqueue::concurrentqueue ALIAS concurrentqueue)
  target_include_directories(concurrentqueue INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/concurrentqueue>")
endif()
