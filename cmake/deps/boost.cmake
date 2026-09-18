set(BOOST_MINOR_MINIMAL 87)
set(BOOST_MINOR_LATEST 100)

unset(BOOST_VERSIONS_LIST)
set(current_val ${BOOST_MINOR_LATEST})
while("${current_val}" GREATER_EQUAL "${BOOST_MINOR_MINIMAL}")
    list(APPEND BOOST_VERSIONS_LIST ${current_val})
    math(EXPR current_val "${current_val} - 1")
endwhile()

foreach(boost_version ${BOOST_VERSIONS_LIST})
  find_package(Boost 1.${boost_version} EXACT GLOBAL QUIET)
  if(Boost_FOUND)
    break()
  endif()
endforeach()

if (NOT Boost_FOUND)
  set(OSSIA_MUST_INSTALL_BOOST 1 CACHE INTERNAL "")

  # Reuse whatever is already unpacked, newest first.
  foreach(boost_version ${BOOST_VERSIONS_LIST})
    if(EXISTS "${OSSIA_3RDPARTY_FOLDER}/boost_1_${boost_version}_0/boost/version.hpp")
      set(BOOST_MINOR_USED ${boost_version})
      break()
    endif()
  endforeach()

  # BOOST_MINOR_LATEST runs ahead of what has actually been released, so walk
  # down until an archive is really there instead of demanding the top of the
  # range. A missing asset still answers 200 with an error page, so the archive
  # only counts as good once it has unpacked into the headers we need.
  if(NOT DEFINED BOOST_MINOR_USED)
    foreach(boost_version ${BOOST_VERSIONS_LIST})
      set(BOOST_ARCHIVE "boost_1_${boost_version}_0.tar.gz")
      set(BOOST_URL "https://github.com/ossia/sdk/releases/download/sdk31/${BOOST_ARCHIVE}")
      message(STATUS "Trying boost archive ${BOOST_URL}")

      file(DOWNLOAD "${BOOST_URL}" "${OSSIA_3RDPARTY_FOLDER}/${BOOST_ARCHIVE}"
           STATUS BOOST_DOWNLOAD_STATUS)

      list(GET BOOST_DOWNLOAD_STATUS 0 BOOST_DOWNLOAD_CODE)
      if(NOT BOOST_DOWNLOAD_CODE EQUAL 0)
        file(REMOVE "${OSSIA_3RDPARTY_FOLDER}/${BOOST_ARCHIVE}")
        continue()
      endif()

      execute_process(
        COMMAND "${CMAKE_COMMAND}" -E tar xzf "${BOOST_ARCHIVE}"
        WORKING_DIRECTORY "${OSSIA_3RDPARTY_FOLDER}"
        RESULT_VARIABLE BOOST_UNTAR_RESULT
        OUTPUT_QUIET
        ERROR_QUIET
      )

      if(BOOST_UNTAR_RESULT EQUAL 0
         AND EXISTS "${OSSIA_3RDPARTY_FOLDER}/boost_1_${boost_version}_0/boost/version.hpp")
        set(BOOST_MINOR_USED ${boost_version})
        break()
      endif()

      file(REMOVE "${OSSIA_3RDPARTY_FOLDER}/${BOOST_ARCHIVE}")
    endforeach()
  endif()

  if(NOT DEFINED BOOST_MINOR_USED)
    message(FATAL_ERROR
      "No boost found, and none of 1.${BOOST_MINOR_MINIMAL} .. 1.${BOOST_MINOR_LATEST} "
      "could be downloaded from https://github.com/ossia/sdk/releases/tag/sdk31")
  endif()

  set(BOOST_VERSION "boost_1_${BOOST_MINOR_USED}_0" CACHE INTERNAL "")
  set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}")
  set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}" CACHE INTERNAL "")
  set(Boost_INCLUDE_DIR "${BOOST_ROOT}")
  list(PREPEND CMAKE_FIND_ROOT_PATH "${BOOST_ROOT}")

  find_package(Boost 1.${BOOST_MINOR_USED} REQUIRED GLOBAL)
endif()
if(BOOST_ROOT)
  set(Boost_INCLUDE_DIR "${BOOST_ROOT}" CACHE INTERNAL "")
endif()
add_library(boost INTERFACE IMPORTED GLOBAL)
set_property(TARGET boost PROPERTY
             INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}")
