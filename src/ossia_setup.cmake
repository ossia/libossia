
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
    BOOST_MATH_DISABLE_FLOAT128=1
    BOOST_ASIO_DISABLE_CONCEPTS=1       # TODO boostorg/asio#312
    $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING>
    $<$<CONFIG:Debug>:BOOST_MULTI_INDEX_ENABLE_SAFE_MODE>
  )

# Workaround for boost being broken with clang 13 (at least until 1.77)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0)
    target_compile_definitions(ossia
      PUBLIC
        BOOST_ASIO_HAS_STD_INVOKE_RESULT=1
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

  target_link_libraries(ossia PRIVATE ws2_32 winmm)
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
target_link_libraries(ossia PUBLIC Threads::Threads)

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
set_target_properties(ossia PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN 1
    )
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

target_include_directories(ossia SYSTEM
  PUBLIC
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/variant/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/mparkvariant/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/nano-signal-slot/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/spdlog/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/brigand/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/fmt/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/hopscotch-map/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/mdspan/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/GSL/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/tuplet/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/flat_hash_map>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/flat>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/readerwriterqueue>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/concurrentqueue>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/SmallFunction/smallfun/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/websocketpp>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/dr_libs>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/rnd/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/tuplet/include>

      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/rapidjson/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/libremidi/include>
      $<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/oscpack>

      $<INSTALL_INTERFACE:include>
  )
