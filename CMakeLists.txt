set(CMAKE_MODULE_PATH
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_MODULE_PATH}"
)

if(OSSIA_SDK)
  list(APPEND CMAKE_PREFIX_PATH "${OSSIA_SDK}/portaudio/lib/cmake/portaudio")
endif()
cmake_minimum_required(VERSION 3.25...3.31 FATAL_ERROR)
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

project(ossia C CXX)

set(OSSIA_IS_TOP_LEVEL "${PROJECT_IS_TOP_LEVEL}")
set(CMAKE_POSITION_INDEPENDENT_CODE 1)
find_package(Threads)

if(NOT QT_VERSION)
  set(QT_VERSION Qt6 6.4)
  set(QT_PREFIX Qt6)
endif()
find_package(${QT_VERSION} COMPONENTS Core)

# Options
## FIXME : the following variable doesn't have a type
## BUT cmake seems to suffer from a bug here
## with `set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty" CACHE PATH "" INTERNAL)`
## the variable looks like : `/tmp/DpvCam/3rdparty/libossia/3rdparty;CACHE;STRING;;INTERNAL`
## which is a list instead of a single PATH value
## at least with CMake 15.2
set(OSSIA_SOURCE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "")
set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty" CACHE INTERNAL "")

if(WIN32)
  # Pin the Windows target for every translation unit, whatever the compiler.
  #
  # <windows.h> defines _WIN32_WINNT itself, through <sdkddkver.h>, whenever it
  # is not already set, so leaving it alone does not mean "no minimum" - it
  # means each translation unit gets one depending on whether it reached
  # <windows.h> at all. Headers that branch on it, Boost.Asio among them, then
  # configure themselves differently from one file to the next: Asio reads it
  # to decide BOOST_ASIO_HAS_STD_ATOMIC_WAIT, which changes the wait primitive
  # it uses. That is an ODR violation, and nothing reports it because both
  # spellings mangle the same.
  #
  # The defaults also disagree between toolchains: the Windows Kits header
  # picks 0x0A00 while mingw-w64 picks _WIN32_WINNT_WS03, so a mingw build has
  # been configuring itself for Server 2003 wherever this was left alone.
  #
  # Here rather than on the ossia target, and before any add_subdirectory: the
  # dependencies we add as subdirectories compile Asio too - libremidi builds
  # its own translation units on MSVC - and they are siblings of ossia rather
  # than consumers of it, so a PUBLIC definition never reaches them. It is
  # repeated in src/ossia_setup.cmake as PUBLIC so that projects consuming an
  # installed ossia get it too.
  #
  # WINVER and NTDDI_VERSION are deliberately not set: sdkddkver.h derives
  # WINVER from _WIN32_WINNT and NTDDI_VERSION from the SDK, in both
  # toolchains, so setting them by hand only creates a way for them to
  # disagree.
  # And give Boost.Asio's global symbols a version-tagged name.
  #
  # Since 1.91 they are named through BOOST_ASIO_VERSIONED_NAME, which with no
  # version namespace expands to the bare asio_ prefix - the names standalone
  # Asio uses. Where Asio is compiled separately we emit strong definitions of
  # them, so anything else in the link carrying its own standalone Asio
  # collides: score's LSL addon compiles asio/impl/src.hpp, and the two meet as
  # a duplicate asio_signal_handler.
  #
  # The tag encodes the Asio configuration, which is why this only holds
  # together with the pin above; without it the namespace varies per TU and the
  # duplicate symbol merely becomes an undefined one. It also requires that
  # nothing forward-declares an Asio type in plain boost::asio, which would be
  # ambiguous against the real one - see libremidi's backends/net/config.hpp,
  # which declares io_context inside the same inline namespace we do.
  #
  # Standalone Asio has no equivalent knob, still hardcoding the bare names as
  # of 1.36, so the versioning has to come from our side. A no-op before 1.91.
  add_definitions(
    -D_WIN32_WINNT=0x0A00
    -DBOOST_ASIO_ENABLE_VERSION_NAMESPACE=1
  )
endif()

include(OssiaOptions)

# Dependencies
include(OssiaDeps)

# Configuration
include(OssiaConfiguration)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0056 NEW)
cmake_policy(SET CMP0069 NEW)

# Main library
add_subdirectory(src)

# Examples & tests
if(OSSIA_EXAMPLES)
    add_subdirectory(examples)
endif()

if(OSSIA_TESTING)
  set(CMAKE_CTEST_ARGUMENTS --rerun-failed --output-on-failure)
  include(CTest)
  ossia_use_system(use_sys Catch2)
  if(use_sys)
    find_package(Catch2 CONFIG REQUIRED)
    if(NOT TARGET Catch2::Catch2WithMain)
      add_library(Catch2WithMain INTERFACE)
      add_library(Catch2::Catch2WithMain ALIAS Catch2WithMain)
      target_compile_definitions(Catch2WithMain INTERFACE CATCH_CONFIG_MAIN)
      target_link_libraries(Catch2WithMain INTERFACE Catch2::Catch2)
    endif()
  else()
    set(CATCH_INSTALL_DOCS 0)
    set(CATCH_INSTALL_DOCS 0 CACHE INTERNAL "" FORCE)
    set(CATCH_INSTALL_EXTRAS 0)
    set(CATCH_INSTALL_EXTRAS 0 CACHE INTERNAL "" FORCE)
    set(CATCH_BUILD_TESTING 0)
    set(CATCH_BUILD_TESTING 0 CACHE INTERNAL "" FORCE)
    set(CATCH_BUILD_STATIC_LIBRARY 1)
    set(CATCH_BUILD_STATIC_LIBRARY 1 CACHE INTERNAL "" FORCE)

    set(old_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS 0)
    add_subdirectory(3rdparty/Catch2)
    set(BUILD_SHARED_LIBS "${old_BUILD_SHARED_LIBS}")
  endif()
  add_subdirectory(tests)
endif()

if(OSSIA_GENVAR)
  add_subdirectory(tools/gen_variant)
endif()
