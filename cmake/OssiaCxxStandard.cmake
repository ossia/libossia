# The language standard has to be chosen before the dependencies are added:
# add_subdirectory() freezes CMAKE_CXX_STANDARD and CMAKE_CXX_FLAGS as they
# stand, so anything pulled in from OssiaDeps would otherwise pick a standard of
# its own. That is not only an inconsistency on paper -- boost.asio tags its
# inline namespace with the language features it can see, so two standards in
# one build give two incompatible asio ABIs that then fail to link.
#
# Only the standard and /Zc:__cplusplus live here. The rest of our flags stay in
# OssiaConfiguration, after the dependencies, so that we do not impose
# /permissive- or our warning settings on third-party code.
include(CheckCXXCompilerFlag)

if(MSVC)
  # MSVC reports __cplusplus as 199711L without this, whatever /std: says, and
  # boost.asio decides part of its ABI namespace from __cplusplus.
  set(CMAKE_CXX_FLAGS "/Zc:__cplusplus ${CMAKE_CXX_FLAGS}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
  endif()
  set(CMAKE_CXX_FLAGS "-clang:-std:c++${OSSIA_CXX_STANDARD} ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
  if(OSSIA_TESTING OR OSSIA_EXAMPLES)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_FLAGS "/std:c++latest ${CMAKE_CXX_FLAGS}")
  elseif(OSSIA_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD ${OSSIA_CXX_STANDARD})
    set(CMAKE_CXX_FLAGS "/std:c++${OSSIA_CXX_STANDARD} ${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "/std:c++20 ${CMAKE_CXX_FLAGS}")
  endif()
else()
  if(CMAKE_VERSION VERSION_GREATER 3.30)
    check_cxx_compiler_flag(-std=c++26 has_std_26_flag)
  endif()
  if(CMAKE_VERSION VERSION_GREATER 3.20)
    check_cxx_compiler_flag(-std=c++2b has_std_2b_flag)
  endif()
  check_cxx_compiler_flag(-std=c++20 has_std_20_flag)
  check_cxx_compiler_flag(-std=c++2a has_std_2a_flag)
  check_cxx_compiler_flag(-std=c++17 has_std_17_flag)
  check_cxx_compiler_flag(-std=c++1z has_std_1z_flag)

  if(OSSIA_CXX_STANDARD)
    if("${OSSIA_CXX_STANDARD}" EQUAL 26)
      set(CMAKE_CXX_STANDARD 26)
      set(CXX_STANDARD_FLAG -std=c++26)
    elseif("${OSSIA_CXX_STANDARD}" EQUAL 23)
        set(CMAKE_CXX_STANDARD 23)
        set(CXX_STANDARD_FLAG -std=c++2b)
    elseif("${OSSIA_CXX_STANDARD}" EQUAL 20)
        set(CMAKE_CXX_STANDARD 20)
        set(CXX_STANDARD_FLAG -std=c++20)
    else()
      set(CMAKE_CXX_STANDARD ${OSSIA_CXX_STANDARD})
      set(CXX_STANDARD_FLAG "-std=c++${OSSIA_CXX_STANDARD}")
    endif()
  elseif(EMSCRIPTEN)
    set(CMAKE_CXX_STANDARD 20)
    set(CXX_STANDARD_FLAG -std=c++20)
  elseif (has_std_2b_flag)
    set(CMAKE_CXX_STANDARD 23)
    set(CXX_STANDARD_FLAG -std=c++2b)
  elseif (has_std_20_flag)
    set(CMAKE_CXX_STANDARD 20)
    set(CXX_STANDARD_FLAG -std=c++20)
  elseif (has_std_2a_flag)
    set(CMAKE_CXX_STANDARD 20)
    set(CXX_STANDARD_FLAG -std=c++2a)
  else()
    message(WARNING "check_cxx_compiler_flag was not able to find C++20 or C++23. We will assume C++20")
    set(CMAKE_CXX_STANDARD 20)
    set(CXX_STANDARD_FLAG -std=c++20)
  endif ()
  set(CMAKE_CXX_FLAGS "${CXX_STANDARD_FLAG} ${CMAKE_CXX_FLAGS}")
endif()
