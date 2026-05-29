set(BOOST_MINOR_MINIMAL 87)
# Cap at 1.87: 1.88+ changed parts of Boost.Asio's reactor / handler-tracking
# bookkeeping in a way that interacts badly with the bundled websocketpp on
# macOS arm64 + libc++ Debug, producing SIGBUS in
# reactive_socket_recv_op_base::do_perform when destroying the std::function
# completion handler. websocketpp was last patched for 1.87 (3b86173af, "Fix
# boost 1.87 support") so stay there until websocketpp gets newer asio love.
set(BOOST_MINOR_LATEST 87)

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
  set(BOOST_VERSION "boost_1_${BOOST_MINOR_LATEST}_0" CACHE INTERNAL "")

  if(NOT EXISTS "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}/")
    message(STATUS "Downloading boost to ${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}.tar.gz")
    set(BOOST_URL https://github.com/ossia/sdk/releases/download/sdk31/${BOOST_VERSION}.tar.gz)
    set(BOOST_ARCHIVE ${BOOST_VERSION}.tar.gz)

    file(DOWNLOAD "${BOOST_URL}" "${OSSIA_3RDPARTY_FOLDER}/${BOOST_ARCHIVE}")

    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E tar xzf "${BOOST_ARCHIVE}"
      WORKING_DIRECTORY "${OSSIA_3RDPARTY_FOLDER}"
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
  set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}")
  set(BOOST_ROOT "${OSSIA_3RDPARTY_FOLDER}/${BOOST_VERSION}" CACHE INTERNAL "")
  set(Boost_INCLUDE_DIR "${BOOST_ROOT}")
  list(PREPEND CMAKE_FIND_ROOT_PATH "${BOOST_ROOT}")

  find_package(Boost 1.${BOOST_MINOR_LATEST} REQUIRED GLOBAL)
endif()
if(BOOST_ROOT)
  set(Boost_INCLUDE_DIR "${BOOST_ROOT}" CACHE INTERNAL "")
endif()
add_library(boost INTERFACE IMPORTED GLOBAL)
set_property(TARGET boost PROPERTY
             INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}")
