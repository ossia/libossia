include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)
include(StaticQt)

if(OSSIA_MOST_STATIC)
    set(OSSIA_STATIC ON)
    set(CMAKE_LINK_SEARCH_END_STATIC ON)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
endif()

if(OSSIA_MAX OR OSSIA_PD AND WIN32)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
endif()

# System detection
include(ProcessorCount)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wmisleading-indentation" SUPPORTS_MISLEADING_INDENT_FLAG)
check_cxx_compiler_flag("-Wl,-z,defs" WL_ZDEFS_SUPPORTED)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(LINKER_IS_LLD 0)
  set(LINKER_IS_GOLD 0)
  set(OSSIA_PD 0)
  set(OSSIA_PYTHON 0)
  set(OSSIA_DATAFLOW 0)
  set(OSSIA_PROTOCOL_MIDI 0)
  set(OSSIA_PCH 0)
  set(ANDROID 1)
else()
  if(UNIX AND NOT APPLE)
      find_program(LSB_RELEASE lsb_release)
      if(LSB_RELEASE)
        execute_process(COMMAND ${LSB_RELEASE} -i
            OUTPUT_VARIABLE RELEASE_CODENAME
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      endif()
  endif()

  macro(check_cxx_linker_flag _Flag _Var)
    set(old_link_libs "${CMAKE_REQUIRED_LIBRARIES}")
    set(CMAKE_REQUIRED_LIBRARIES ${old_link_libs} "${_Flag}")
    check_cxx_compiler_flag("${_Flag}" ${_Var})
    set(CMAKE_REQUIRED_LIBRARIES "${old_link_libs}")
  endmacro()

  if(OSSIA_USE_FAST_LINKER)
    check_cxx_linker_flag("-fuse-ld=lld" LLD_LINKER_SUPPORTED)
    check_cxx_linker_flag("-fuse-ld=gold" GOLD_LINKER_SUPPORTED)

    if(OSSIA_SANITIZE AND NOT APPLE)
      set(LLD_LINKER_SUPPORTED 0)
    endif()
    if(LLD_LINKER_SUPPORTED)
      set(LINKER_IS_LLD 1 CACHE INTERNAL "use lld linker")
    elseif(GOLD_LINKER_SUPPORTED)
      set(LINKER_IS_GOLD 1 CACHE INTERNAL "use gold linker")
    endif()

    if(LINKER_IS_GOLD)
      check_cxx_linker_flag("-fuse-ld=gold -Wl,--threads -Wl,--thread-count,2" LINKER_THREADS_SUPPORTED)
      check_cxx_linker_flag("-fuse-ld=gold -Wl,--gdb-index" GDB_INDEX_SUPPORTED)
    elseif(LINKER_IS_LLD)
      check_cxx_linker_flag("-fuse-ld=lld -Wl,--threads" LINKER_THREADS_SUPPORTED)
      check_cxx_linker_flag("-fuse-ld=lld -Wl,--gdb-index" GDB_INDEX_SUPPORTED)
    endif()
  else()
    check_cxx_linker_flag("-Wl,--gdb-index" GDB_INDEX_SUPPORTED)
  endif()
endif()

if(OSSIA_SPLIT_DEBUG)
  set(DEBUG_SPLIT_FLAG "-gsplit-dwarf")
  if(NOT APPLE AND NOT MINGW)
    set(GOLD_FLAGS
#      -Wa,--compress-debug-sections
#      -Wl,--compress-debug-sections=zlib
      -Wl,--dynamic-list-cpp-new
      -Wl,--dynamic-list-cpp-typeinfo
    )
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
      set(GOLD_FLAGS ${GOLD_FLAGS} -Wno-unused-command-line-argument)
    endif()
  endif()

  if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(GOLD_FLAGS ${GOLD_FLAGS}
      -ggdb
    )
  endif()

  if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(GOLD_FLAGS ${GOLD_FLAGS}
      -ggdb
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
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_FLAGS "/std:c++17 ${CMAKE_CXX_FLAGS}")
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
  endif ()
  set(CMAKE_CXX_FLAGS "${CXX_STANDARD_FLAG} ${CMAKE_CXX_FLAGS}")
endif()

# So that make install after make all_unity does not rebuild everything :
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)

# We disable debug infos on OS X on travis because it takes up too much space
if(OSSIA_CI AND APPLE OR OSSIA_NO_DEBUG_INFO)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g0")
endif()

if(OSSIA_CI)
  set(OSSIA_USE_FAST_LINKER ON)
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

  set(OSSIA_LINK_OPTIONS
    -ffunction-sections
    -fdata-sections
  )

  if(CMAKE_COMPILER_IS_GNUCXX)
    set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
      -fvar-tracking-assignments
    )
    if(NOT OSSIA_SPLIT_DEBUG)
      set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
        -Wl,-Bsymbolic-functions
      )
    endif()

    if(GDB_INDEX_SUPPORTED AND NOT OSSIA_SANITIZE)
      set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
          ${DEBUG_SPLIT_FLAG}
          -Wl,--gdb-index
      )
    endif()
  endif()

  if(LINKER_IS_GOLD)
    set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} ${GOLD_FLAGS})
  endif()

  if(OSSIA_MOST_STATIC)
    set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -static -static-libgcc -static-libstdc++)
  endif()

  if(OSSIA_CI)
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
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
          set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -Wl,-S)
    endif()
  endif()

  if(NOT WIN32)
    if(NOT APPLE)
      set(OSSIA_LINK_OPTIONS
        -Wl,--gc-sections
        -Wl,--as-needed
      )
    else()
      set(OSSIA_LINK_OPTIONS
        -Wl,-dead_strip
      )
    endif()
  endif()

  if("${SUPPORTS_MISLEADING_INDENT_FLAG}")
      set(OSSIA_COMPILE_OPTIONS ${OSSIA_COMPILE_OPTIONS} -Wmisleading-indentation)
  endif()

#    if(APPLE)
#        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-warn_weak_exports")
#        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-warn_weak_exports")
#        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-warn_weak_exports")
#    endif()
endif()

if(OSSIA_LTO)
  setup_lto()
endif()
