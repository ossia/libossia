
# Useful properties
if (OSSIA_EDITOR)
  source_group(Editor REGULAR_EXPRESSION "ossia/editor.*")
endif()
source_group(Network REGULAR_EXPRESSION "ossia/network.*")

# Compile options
target_compile_definitions(ossia
  PUBLIC
    RAPIDJSON_HAS_STDSTRING=1
    TINYSPLINE_DOUBLE_PRECISION
    BOOST_NO_RTTI=1
    BOOST_MATH_DISABLE_FLOAT128=1
    $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING>
    $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_SAFE_MODE>
  )

if(Boost_VERSION VERSION_LESS 1.81)
  target_compile_definitions(ossia
    PUBLIC
      BOOST_ASIO_DISABLE_CONCEPTS=1
  )
endif()

# Workaround for boost being broken with clang 13 (at least until 1.77)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)
    target_compile_definitions(ossia
      PUBLIC
        BOOST_ASIO_HAS_STD_INVOKE_RESULT=1
    )
  endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0)
    target_compile_options(ossia
      PUBLIC
        -fconcepts
    )
  endif()
endif()

if(WIN32)
  if(MSVC)
    target_compile_definitions(ossia PUBLIC
      _HAS_AUTO_PTR_ETC=1
      _HAS_DEPRECATED_NEGATORS=1 # boost.graph needs std::not1...
      _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS=1

      # Boost.Asio separate compilation only enabled on windows due to
      # https://github.com/chriskohlhoff/asio/issues/820
      BOOST_ASIO_SEPARATE_COMPILATION=1
    )
  endif()

  target_compile_definitions(ossia PUBLIC
    # Pin the Windows target for every translation unit, whatever the compiler.
    #
    # <windows.h> defines _WIN32_WINNT itself, through <sdkddkver.h>, whenever
    # it is not already set, so leaving it alone does not mean "no minimum" - it
    # means each TU gets one depending on whether it reached <windows.h> first.
    # Asio reads it to decide BOOST_ASIO_HAS_STD_ATOMIC_WAIT, so two TUs here
    # can disagree about which wait primitive it uses: an ODR violation that
    # nothing reports, because both spellings mangle the same.
    #
    # The defaults also disagree between toolchains - the Windows Kits header
    # picks 0x0A00, mingw-w64 picks _WIN32_WINNT_WS03 - so a mingw build has
    # been configuring itself for Server 2003 wherever this was left alone.
    #
    # WINVER and NTDDI_VERSION are deliberately not set: sdkddkver.h derives
    # WINVER from _WIN32_WINNT and NTDDI_VERSION from the SDK, so setting them
    # by hand only creates a way for them to disagree.
    _WIN32_WINNT=0x0A00
  )

  # Not BOOST_ASIO_ENABLE_VERSION_NAMESPACE, tempting as it looks.
  #
  # Since Boost 1.91, Asio's global symbols are named through
  # BOOST_ASIO_VERSIONED_NAME, which with no version namespace expands to the
  # bare asio_ prefix - the names standalone Asio uses - so a link that carries
  # both, as score does through its LSL addon, gets a duplicate
  # asio_signal_handler. Enabling the version namespace renames ours and
  # settles that.
  #
  # It cannot be used here: the namespace is an inline namespace, and anything
  # that forward-declares an Asio type declares it in plain boost::asio, which
  # is then ambiguous against the real one. libremidi does exactly that in
  # backends/net/config.hpp, and every translation unit including it fails with
  # C2872 'io_context': ambiguous symbol. Making it work would mean every
  # forward declaration reachable from here, in our own code and in our
  # dependencies, going through BOOST_ASIO_INLINE_NAMESPACE_BEGIN/END.

  if(NOT OSSIA_STATIC)
    target_compile_definitions(ossia
      PUBLIC
        BOOST_ASIO_DYN_LINK=1
    )
  endif()

  target_compile_definitions(ossia PUBLIC
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
  )

  target_link_libraries(ossia PRIVATE ws2_32 winmm bcrypt)
  if(MINGW)
      target_link_libraries(ossia PRIVATE mswsock)
  endif()

  if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" OR OSSIA_UNITY3D)
    set_target_properties(ossia PROPERTIES OUTPUT_NAME "ossia$<$<CONFIG:Debug>:d>")
  else()
    set_target_properties(ossia PROPERTIES OUTPUT_NAME "ossia_x64$<$<CONFIG:Debug>:d>")
  endif()
else()
  # On windows this is already set by the boost headers which gives a macro redefinition warning
  target_compile_definitions(ossia
    PUBLIC
      $<$<CONFIG:Debug>:BOOST_ASIO_ENABLE_BUFFER_DEBUGGING>
  )
endif()

set(THREADS_PREFER_PTHREAD_FLAG)
find_package(Threads REQUIRED)
target_link_libraries(ossia PUBLIC $<BUILD_INTERFACE:Threads::Threads>)

target_compile_options(ossia PRIVATE ${OSSIA_COMPILE_OPTIONS})
target_link_libraries(ossia PRIVATE ${OSSIA_LINK_OPTIONS})

if(BUILD_SHARED_LIBS AND NOT "${OSSIA_STATIC}")
  set_target_properties(ossia PROPERTIES
      C_VISIBILITY_PRESET hidden
      CXX_VISIBILITY_PRESET hidden
      VISIBILITY_INLINES_HIDDEN 1
      )
  if(${OSSIA_NO_SONAME})
      #set_target_properties(ossia PROPERTIES NO_SONAME 1)
  else()
    set_target_properties(ossia PROPERTIES
      VERSION ${ossia_VERSION}
      SOVERSION 1
      )
  endif()

else()
  set_target_properties(ossia PROPERTIES
      VERSION ${ossia_VERSION}
      C_VISIBILITY_PRESET default
      CXX_VISIBILITY_PRESET default
      VISIBILITY_INLINES_HIDDEN 0)
endif()

if(OSSIA_HIDE_ALL_SYMBOLS)
  if("${OSSIA_HIDE_ALL_SYMBOLS}" MATCHES "(hidden|internal)")
    set_target_properties(ossia PROPERTIES
        C_VISIBILITY_PRESET "${OSSIA_HIDE_ALL_SYMBOLS}"
        CXX_VISIBILITY_PRESET "${OSSIA_HIDE_ALL_SYMBOLS}"
        VISIBILITY_INLINES_HIDDEN 1
        )
  else()
    set_target_properties(ossia PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN 1
        )
  endif()
endif()

include(GenerateStaticExport)

if(OSSIA_STATIC)
  target_compile_definitions(ossia PRIVATE "OSSIA_EXPORTS")
endif()
set_target_properties(ossia PROPERTIES DEFINE_SYMBOL "OSSIA_EXPORTS")

if(OSSIA_STATIC_EXPORT)
  if(NOT OSSIA_NO_DLLIMPORT)
    generate_export_header(ossia BASE_NAME OSSIA ALWAYS_EXPORT DLLIMPORT)
  else()
    generate_export_header(ossia BASE_NAME OSSIA ALWAYS_EXPORT)
  endif()
  target_compile_definitions(ossia PRIVATE OSSIA_EXPORTS=1)
else()
  generate_export_header(ossia BASE_NAME OSSIA)
endif()

configure_file(ossia-config.hpp.in ossia-config.hpp)

target_include_directories(ossia
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(ossia
  PRIVATE
    $<BUILD_INTERFACE:rapidfuzz::rapidfuzz>
  PUBLIC
    $<BUILD_INTERFACE:ctre::ctre>
    $<BUILD_INTERFACE:fmt::fmt>
    $<BUILD_INTERFACE:nanosignal::nanosignal>
    $<BUILD_INTERFACE:mdspan::mdspan>
    $<BUILD_INTERFACE:tuplet::tuplet>
    $<BUILD_INTERFACE:readerwriterqueue::readerwriterqueue>
    $<BUILD_INTERFACE:concurrentqueue::concurrentqueue>
    $<BUILD_INTERFACE:websocketpp::websocketpp>
    $<BUILD_INTERFACE:rapidhash::rapidhash>
    $<BUILD_INTERFACE:rapidjson::rapidjson>
    $<BUILD_INTERFACE:re2::re2>
    $<BUILD_INTERFACE:smallfun::smallfun>
    $<BUILD_INTERFACE:span::span>
    $<BUILD_INTERFACE:spdlog::spdlog>
    $<BUILD_INTERFACE:tuplet::tuplet>
    $<BUILD_INTERFACE:unordered_dense::unordered_dense>
)

if(OSSIA_DATAFLOW)
  target_link_libraries(ossia
    PUBLIC
      $<BUILD_INTERFACE:dr_libs::dr_libs>
      $<BUILD_INTERFACE:perlinnoise::perlinnoise>
      $<BUILD_INTERFACE:rnd::rnd>
  )
endif()

target_include_directories(ossia SYSTEM
  PUBLIC
    $<INSTALL_INTERFACE:include>
)
