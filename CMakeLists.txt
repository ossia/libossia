cmake_minimum_required(VERSION 3.5)
project(OSSIA)

# Options
option(OSSIA_SANITIZE "Sanitize build" OFF)
option(OSSIA_TIDY "Use clang-tidy" OFF)
option(OSSIA_STATIC "Make a static build" OFF)
option(OSSIA_COVERAGE "Run code coverage" OFF)
option(OSSIA_EXAMPLES "Build examples" OFF)
option(OSSIA_TESTING "Build tests" OFF)
option(OSSIA_CI "Continuous integration run" OFF)
option(OSSIA_FRAMEWORK "Build an OS X framework" OFF)
option(OSSIA_JAVA "Build JNI bindings" OFF)
option(OSSIA_DISABLE_COTIRE "Disable cotire" OFF)
# Configuration
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0063 NEW)
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wmisleading-indentation" SUPPORTS_MISLEADING_INDENT_FLAG)
check_cxx_compiler_flag("-Wl,-z,defs" WL_ZDEFS_SUPPORTED)
check_cxx_compiler_flag("-fuse-ld=gold" GOLD_LINKER_SUPPORTED)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_CXX_STANDARD 14)

# So that make install after make all_unity does not rebuild everything :
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY True)

if(CMAKE_SYSTEM_NAME MATCHES Emscripten)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/CMake;${PROJECT_SOURCE_DIR}/CMake/cmake-modules;")

if(OSSIA_CI)
  if(APPLE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1 -gline-tables-only")
  endif()
endif()

if(OSSIA_STATIC)
  set(BUILD_SHARED_LIBS OFF)
  set(OSSIA_FRAMEWORK OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

if(OSSIA_COVERAGE)
  include("${CMAKE_CURRENT_LIST_DIR}/CMake/CodeCoverage.cmake")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_COVERAGE}")
endif()
if(MSVC)
    set(OSSIA_LINK_OPTIONS
        "/PDBCompress"
        "/OPT:REF"
        "/OPT:ICF"
        )
    set(OSSIA_COMPILE_OPTIONS
        "/wd4068" # pragma mark -
        "/wd4250" # inherits via dominance
        "/wd4251" # DLL stuff
        "/wd4275" # DLL stuff
        "/wd4244" # return : conversion from foo to bar, possible loss of data
        "/wd4800" # conversion from int to bool, performance warning
        "/wd4503" # decorated name length exceeded
        "/MP"
        "/bigobj"
        ${OSSIA_LINK_OPTIONS}
    )
else()
    set(OSSIA_LINK_OPTIONS
        -ffunction-sections
        -fdata-sections
       # -Wl,-strip_unused
        -Wl,--gc-sections
        -gsplit-dwarf
        -gdwarf-4
        -Wl,--gdb-index
        -fvar-tracking-assignments
        -Wa,--compress-debug-sections
        -Wl,--compress-debug-sections=zlib
        )
    if(OSSIA_CI)
        set(OSSIA_LINK_OPTIONS ${OSSIA_LINK_OPTIONS} -s)
    endif()
    set(OSSIA_COMPILE_OPTIONS
        -Wall
        -Wextra
        -Wno-unused-parameter
        -Wno-unknown-pragmas
        -Wno-missing-braces
        -Wnon-virtual-dtor
        -pedantic
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -pipe
        -Werror=return-type
        -Werror=trigraphs
        -Wmissing-field-initializers
        ${OSSIA_LINK_OPTIONS}
    )

    if("${SUPPORTS_MISLEADING_INDENT_FLAG}")
        set(OSSIA_COMPILE_OPTIONS ${OSSIA_COMPILE_OPTIONS} -Wmisleading-indentation)
    endif()
endif()

# Dependencies
find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

set(OSSIA_3RDPARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty")
if(NOT CMAKE_SYSTEM_NAME MATCHES Emscripten)
    add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/ModernMIDI" EXCLUDE_FROM_ALL)
endif()
add_subdirectory(3rdparty/oscpack EXCLUDE_FROM_ALL)

# Main library
add_subdirectory(OSSIA)

# Examples & tests
if(OSSIA_EXAMPLES)
    add_subdirectory(Documentation/Examples)
endif()

if(OSSIA_TESTING)
    enable_testing()
    add_subdirectory(Tests)
endif()
