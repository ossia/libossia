# Build settings :
option(OSSIA_SANITIZE "Sanitize build" OFF)
option(OSSIA_TIDY "Use clang-tidy" OFF)
option(OSSIA_STATIC "Make a static build" OFF)
option(OSSIA_COVERAGE "Run code coverage" OFF)
option(OSSIA_EXAMPLES "Build examples" OFF)
option(OSSIA_TESTING "Build tests" OFF)
option(OSSIA_CI "Continuous integration run" OFF)
option(OSSIA_FRAMEWORK "Build an OS X framework" OFF)
option(OSSIA_DISABLE_COTIRE "Disable cotire" OFF)
option(OSSIA_NO_SONAME "Set NO_SONAME property" OFF)
option(OSSIA_LTO "Link-time optimizations. Fails on Windows." OFF)
option(OSSIA_OSX_FAT_LIBRARIES "Build 32 and 64 bit fat libraries on OS X" OFF)
option(OSSIA_OSX_RETROCOMPATIBILITY "Build for older OS X versions" OFF)
option(OSSIA_MOST_STATIC "Try to make binaries that are mostly static" OFF)
option(OSSIA_DATAFLOW "Dataflow features" ON)
option(OSSIA_SPLIT_DEBUG "Split debug info" ON)

# Bindings :
option(OSSIA_JAVA "Build JNI bindings" OFF)
option(OSSIA_PD "Build PureData externals" ON)
option(OSSIA_MAX "Build Max/MSP externals" OFF)
option(OSSIA_PYTHON "Build Python bindings" ON)
option(OSSIA_QT "Build Qt bindings" ON)
option(OSSIA_C "Build C bindings" ON)

# Protocols :
option(OSSIA_PROTOCOL_MIDI "Enable MIDI protocol" ON)
option(OSSIA_PROTOCOL_OSCQUERY "Enable OSCQuery protocol" ON)
option(OSSIA_PROTOCOL_HTTP "Enable HTTP protocol" ON) # Requires Qt
option(OSSIA_PROTOCOL_WEBSOCKETS "Enable WebSockets protocol" OFF) # Requires Qt
option(OSSIA_PROTOCOL_SERIAL "Enable Serial port protocol" OFF) # Requires Qt
option(OSSIA_NO_QT "Disable all the features that may require Qt" OFF)
option(OSSIA_DNSSD "Enable DNSSD support" ON)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/CMake;${PROJECT_SOURCE_DIR}/CMake/cmake-modules;")

include(Sanitize)
include(DebugMode)
include(UseGold)
include(LinkerWarnings)

if(OSSIA_MAX)
    set(OSSIA_OSX_FAT_LIBRARIES 1)
    set(OSSIA_NO_QT 1)
endif()
if(OSSIA_OSX_FAT_LIBRARIES)
    set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
    set(OSSIA_DISABLE_COTIRE 1)
endif()
if(OSSIA_NO_QT)
  set(OSSIA_QT 0)
endif()
if(OSSIA_OSX_RETROCOMPATIBILITY)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
endif()


if(OSSIA_MOST_STATIC)
    set(OSSIA_STATIC ON)
    set(CMAKE_LINK_SEARCH_END_STATIC ON)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
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
  set(OSSIA_DNSSD 0)
  set(OSSIA_PROTOCOL_MIDI 0)
  set(OSSIA_DISABLE_COTIRE 1)
  set(ANDROID 1)
else()
  check_cxx_compiler_flag("-fuse-ld=lld" LLD_LINKER_SUPPORTED)
  check_cxx_compiler_flag("-fuse-ld=gold" GOLD_LINKER_SUPPORTED)
  if(LLD_LINKER_SUPPORTED)
    set(LINKER_IS_LLD 1)
  elseif(GOLD_LINKER_SUPPORTED)
    set(LINKER_IS_GOLD 1)
  endif()
endif()

if(OSSIA_SPLIT_DEBUG)
  set(DEBUG_SPLIT_FLAG "-gsplit-dwarf")
set(GOLD_FLAGS
  -Wa,--compress-debug-sections
  -Wl,--compress-debug-sections=zlib
  -Wl,--dynamic-list-cpp-new
  -Wl,--dynamic-list-cpp-typeinfo
)
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
set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
if(${CMAKE_VERSION} VERSION_LESS 3.8.0 OR ANDROID)
  set(CMAKE_CXX_STANDARD 14)
  if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  endif()
else()
  set(CMAKE_CXX_STANDARD 17)
  if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
  endif()
endif()
# So that make install after make all_unity does not rebuild everything :
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)

if(CMAKE_SYSTEM_NAME MATCHES Emscripten)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
endif()

# We disable debug infos on OS X on travis because it takes up too much space
if(OSSIA_CI)
  if(APPLE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g0")
  endif()
endif()

if(OSSIA_STATIC)
  set(BUILD_SHARED_LIBS OFF)
  set(OSSIA_FRAMEWORK OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

if(OSSIA_COVERAGE)
  include(CodeCoverage)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
endif()

# Compiler & linker flags
if(MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /PDBCompress /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /PDBCompress /OPT:REF /OPT:ICF")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /PDBCompress /OPT:REF /OPT:ICF")

    set(OSSIA_COMPILE_OPTIONS
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        "/wd4251" # DLL stuff
        "/wd4275" # DLL stuff
        "/wd4244" # return : conversion from foo to bar, possible loss of data
        "/wd4800" # conversion from int to bool, performance warning
        "/wd4804" # unsafe mix of const bool <= const int
        "/wd4805" # unsafe mix of const bool == const int
        "/wd4996" # SCL_SECURE_NO_WARNINGS
        "/wd4503" # decorated name length exceeded
        "/wd4305" # argument : truncation from double to float
        "/MP"
        "/bigobj"
        ${OSSIA_LINK_OPTIONS}
    )
else()
  if(CMAKE_BUILD_TYPE MATCHES Release)
    set(OSSIA_LINK_OPTIONS
      -ffunction-sections
      -fdata-sections
    )
  endif()

    if(CMAKE_COMPILER_IS_GNUCXX)
      set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
        -fvar-tracking-assignments
      )
    endif()

    if(UNIX AND NOT APPLE AND NOT OSSIA_SPLIT_DEBUG)
      set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
        -Wl,--gc-sections
        -Wl,-Bsymbolic-functions
      )
    endif()

    if(LINKER_IS_GOLD OR LINKER_IS_LLD)
      if(NOT OSSIA_SANITIZE AND NOT OSSIA_SPLIT_DEBUG)
        set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
          ${DEBUG_SPLIT_FLAG}
        )
        if(NOT APPLE)
            set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS}
              -Wl,--gdb-index
            )
        endif()
      endif()
    endif()

    if(LINKER_IS_GOLD)
      set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} ${GOLD_FLAGS})
    endif()

    if(OSSIA_MOST_STATIC)
      set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -static -static-libgcc -static-libstdc++)
    endif()

    if(OSSIA_CI)
        if(CMAKE_COMPILER_IS_GNUCXX)
            set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -s)
        endif()
    endif()

    set(OSSIA_COMPILE_OPTIONS
        ${OSSIA_COMPILE_OPTIONS}
        -std=c++1z
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
    if(NOT CMAKE_COMPILER_IS_GNUCXX)
      set(OSSIA_COMPILE_OPTIONS ${OSSIA_COMPILE_OPTIONS}
        -Wno-cast-align
        -Wno-unused-local-typedef)
    endif()
    if(OSSIA_CI)
        if(NOT CMAKE_COMPILER_IS_GNUCXX)
            set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -Wl,-S)
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
