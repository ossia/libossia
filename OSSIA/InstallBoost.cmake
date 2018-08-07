if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/boost)
  set(BOOST_ARCHIVE "https://github.com/OSSIA/score-sdk/releases/download/sdk9/boost.tar.gz")
  message("Download boost header for dev package from ${BOOST_ARCHIVE} to ${CMAKE_CURRENT_BINARY_DIR}/boost.tar.gz")
  file(DOWNLOAD
    ${BOOST_ARCHIVE}
    ${CMAKE_BINARY_DIR}/boost.tar.gz)

  message("Then unpack it...")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf boost.tar.gz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
