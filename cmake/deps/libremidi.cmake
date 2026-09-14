block()
  set(LIBREMIDI_EXAMPLES OFF CACHE "" INTERNAL)
  set(LIBREMIDI_TESTS OFF CACHE "" INTERNAL)

  # Still does not build with clang
  set(LIBREMIDI_NO_WINUWP ON)
  set(LIBREMIDI_NO_WINUWP ON CACHE "" INTERNAL)
  if(EMSCRIPTEN)
    set(LIBREMIDI_HEADER_ONLY ON CACHE "" INTERNAL)
  else()
    set(LIBREMIDI_HEADER_ONLY OFF CACHE "" INTERNAL)
  endif()
  set(WEAKJACK_FOLDER "${OSSIA_3RDPARTY_FOLDER}")
  set(BUILD_SHARED_LIBS 0)
  add_definitions(
    -DBOOST_NO_RTTI=1
    -DBOOST_MATH_DISABLE_FLOAT128=1
  )

  # libremidi compiles boost.asio too, and BOOST_ASIO_ENABLE_BUFFER_DEBUGGING
  # changes asio's internal types: every target compiling boost.asio in this
  # build must agree on it, or the binary holds two incompatible definitions of
  # the same asio templates -- an ODR violation.
  # Same condition as ossia_setup.cmake: on Windows the boost headers already
  # define it, and defining it again warns.
  if(NOT WIN32)
    add_compile_definitions($<$<CONFIG:Debug>:BOOST_ASIO_ENABLE_BUFFER_DEBUGGING>)
  endif()

  if(WIN32)
    if(MSVC)
      add_definitions(-DBOOST_ASIO_SEPARATE_COMPILATION=1)
    endif()
  endif()
  add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/libremidi" EXCLUDE_FROM_ALL)
endblock()
