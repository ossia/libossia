include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)
include(StaticQt)

if(MSVC)
  if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "EHsc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  endif()
endif()

# System detection
include(ProcessorCount)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wmisleading-indentation" SUPPORTS_MISLEADING_INDENT_FLAG)
check_cxx_compiler_flag("-Wl,-z,defs" WL_ZDEFS_SUPPORTED)

# iOS
include(${OSSIA_3RDPARTY_FOLDER}/ios-cmake/helper-macros.cmake)

if(PLATFORM)
    set(OSSIA_PLATFORM ios)
    # Add some sanitary checks that the toolchain is actually working!
    include(CheckCXXSymbolExists)
    check_cxx_symbol_exists(kqueue sys/event.h HAVE_KQUEUE)
    if(NOT HAVE_KQUEUE)
      message(FATAL_ERROR "kqueue NOT found!")
    endif()

    # Hook up XCTest for the supported plaforms (all but WatchOS)
    if(NOT PLATFORM MATCHES ".*WATCHOS.*")
      # Use the standard find_package, broken between 3.14.0 and 3.14.4 at least for XCtest...
      find_package(XCTest REQUIRED)
      # Fallback: Try to find XCtest as host package via toochain macro (should always work)
      # find_host_package(XCTest REQUIRED)
    endif()
    set(OSSIA_STATIC ON) # build only static lib for iOS
else()
    set(OSSIA_PLATFORM macos)
endif()

if(UNIX AND NOT APPLE)
  find_program(LSB_RELEASE lsb_release)
  if(LSB_RELEASE)
    execute_process(COMMAND ${LSB_RELEASE} -i
      OUTPUT_VARIABLE RELEASE_CODENAME
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
  endif()
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*arm.*")
    set(OSSIA_ARCHITECTURE arm)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*aarch64.*")
    set(OSSIA_ARCHITECTURE arm)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*64.*")
    set(OSSIA_ARCHITECTURE amd64)
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES ".*86.*")
    set(OSSIA_ARCHITECTURE x86)
else()
    message("Could not determine target architecture")
    return()
endif()

# Common setup
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
  if(OSSIA_TESTING OR OSSIA_EXAMPLES)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "/EHsc /MP /std:c++latest /Zc:__cplusplus /permissive- /volatile:iso ${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "/std:c++17 ${CMAKE_CXX_FLAGS}")
  endif()
else()
  if(NOT APPLE AND NOT WIN32 AND CMAKE_VERSION VERSION_GREATER 3.16)
    check_cxx_compiler_flag(-std=c++20 has_std_20_flag)
    check_cxx_compiler_flag(-std=c++2a has_std_2a_flag)
  endif()
  check_cxx_compiler_flag(-std=c++17 has_std_17_flag)
  check_cxx_compiler_flag(-std=c++1z has_std_1z_flag)

  if (has_std_20_flag)
    set(CMAKE_CXX_STANDARD 20)
    set(CXX_STANDARD_FLAG -std=c++20)
  elseif (has_std_2a_flag)
    set(CMAKE_CXX_STANDARD 20)
    set(CXX_STANDARD_FLAG -std=c++2a)
  elseif (has_std_17_flag)
    set(CMAKE_CXX_STANDARD 17)
    set(CXX_STANDARD_FLAG -std=c++17)
  elseif (has_std_1z_flag)
    set(CMAKE_CXX_STANDARD 17)
    set(CXX_STANDARD_FLAG -std=c++1z)
  else()
    message(WARNING "check_cxx_compiler_flag was not able to find C++17 or C++20. We will assume C++20")
    set(CMAKE_CXX_STANDARD 20)
    set(CXX_STANDARD_FLAG -std=c++20)
  endif ()
  set(CMAKE_CXX_FLAGS "${CXX_STANDARD_FLAG} ${CMAKE_CXX_FLAGS}")
endif()

# So that make install after make all_unity does not rebuild everything :
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)

# We disable debug infos on OS X on travis because it takes up too much space
if(OSSIA_CI AND APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g0")
endif()

if(OSSIA_STATIC)
  set(BUILD_SHARED_LIBS OFF)
  set(OSSIA_FRAMEWORK OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

# Compiler & linker flags
if(MSVC)
  set(CMAKE_CXX_FLAGS "-DNOGDI -DLF_FACESIZE=32 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /PDBCompress /OPT:REF /OPT:ICF")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /PDBCompress /OPT:REF /OPT:ICF")
  set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /PDBCompress /OPT:REF /OPT:ICF")

  set(OSSIA_COMPILE_OPTIONS
      "/wd4065" # switch statement contains default but no case labels
      "/wd4068" # pragma mark -
      "/wd4221" # this object file does not define any previously undefined public symbols
      "/wd4250" # inherits via dominance
      "/wd4251" # DLL stuff
      "/wd4267" # initializing: conversion from size_t to int, possible loss of data or 'argument': conversion from size_t to ..., possible loss of data
      "/wd4275" # DLL stuff
      "/wd4244" # return : conversion from foo to bar, possible loss of data
      "/wd4305" # argument : truncation from double to float
      "/wd4503" # decorated name length exceeded
      "/wd4624" # destructor was implicityl defined as deleted
      "/wd4800" # conversion from int to bool, performance warning
      "/wd4804" # unsafe mix of const bool <= const int
      "/wd4805" # unsafe mix of const bool == const int
      "/wd4996" # SCL_SECURE_NO_WARNINGS
      "/bigobj"
      ${OSSIA_LINK_OPTIONS}
  )
else()
  if(CMAKE_BUILD_TYPE MATCHES Release)
    set(OSSIA_COMPILE_OPTIONS
      ${OSSIA_COMPILE_OPTIONS}
      -fno-math-errno
    )
  endif()

  if(CMAKE_COMPILER_IS_GNUCXX)
    set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
      -fvar-tracking-assignments
    )

    # Note: this may fail with -gsplit-dwarf
    set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
      -Wl,-Bsymbolic-functions
    )
  endif()

  if(OSSIA_CI AND NOT OSSIA_STATIC_EXPORT)
    set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -s)
  endif()

  if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(OSSIA_COMPILE_OPTIONS ${OSSIA_COMPILE_OPTIONS}
      -Wno-gnu-statement-expression
      -Wno-four-char-constants
      -Wno-cast-align
      -Wno-unused-local-typedef
      #-Wweak-vtables
    )
  endif()
  set(OSSIA_COMPILE_OPTIONS
      ${OSSIA_COMPILE_OPTIONS}
      -Wall
      -Wextra
      -Wno-unused-parameter
      -Wno-unknown-pragmas
      -Wno-missing-braces
      -Wnon-virtual-dtor
      -pedantic
      -Wunused
      -Woverloaded-virtual
      -pipe
      -Werror=return-type
      -Werror=trigraphs
      -Wmissing-field-initializers
      ${OSSIA_LINK_OPTIONS}
  )
  if(APPLE)
    set(OSSIA_COMPILE_OPTIONS
        ${OSSIA_COMPILE_OPTIONS}
        -Wno-auto-var-id
        -Wno-availability
        -Wno-deprecated-declarations
        -Wno-exceptions
        -Wno-auto-var-id
        -Wno-availability
        -Wno-deprecated-declarations
        -Wno-exceptions
        -Wno-extra-semi
        -Wno-gnu-folding-constant
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-inconsistent-missing-override
        -Wno-infinite-recursion
        -Wno-missing-method-return-type
        -Wno-non-virtual-dtor
        -Wno-nullability-completeness-on-arrays
        -Wno-nullability-extension
        -Wno-pedantic
        -Wno-sign-compare
        -Wno-switch
        -Wno-unguarded-availability-new
        -Wno-unknown-warning-option
        -Wno-unused-function
        -Wno-unused-local-typedef
        -Wno-unused-private-field
        -Wno-unused-variable
        -Wno-variadic-macros
        -Wno-zero-length-array
    )
  endif()

  if(OSSIA_CI)
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND NOT OSSIA_STATIC_EXPORT)
      set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -Wl,-S)
    endif()
  endif()

  if("${SUPPORTS_MISLEADING_INDENT_FLAG}")
      set(OSSIA_COMPILE_OPTIONS ${OSSIA_COMPILE_OPTIONS} -Wmisleading-indentation)
  endif()
endif()
