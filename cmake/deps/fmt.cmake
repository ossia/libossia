if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(fmt CONFIG GLOBAL)
endif()

if(NOT TARGET fmt::fmt)
  set(OSSIA_FMT_INTERNAL 1 CACHE "" INTERNAL)

  if(OSSIA_FAST_DEVELOPER_BUILD)
    add_library(fmt SHARED
      "${OSSIA_3RDPARTY_FOLDER}/fmt/src/format.cc"
      "${OSSIA_3RDPARTY_FOLDER}/fmt/src/os.cc"
    )
    target_compile_definitions(fmt
      PRIVATE
        FMT_LIB_EXPORT=1
      PUBLIC
        FMT_USE_LONG_DOUBLE=0
        FMT_USE_INT128=0
        FMT_USE_FLOAT128=0
        FMT_STATIC_THOUSANDS_SEPARATOR=1
      INTERFACE
        FMT_SHARED=1
    )
    target_include_directories(fmt PUBLIC "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/fmt/include>")
  else()
    add_library(fmt INTERFACE IMPORTED GLOBAL)
    target_compile_definitions(fmt
      INTERFACE
        FMT_HEADER_ONLY=1

        FMT_USE_LONG_DOUBLE=0
        FMT_USE_INT128=0
        FMT_USE_FLOAT128=0
        FMT_STATIC_THOUSANDS_SEPARATOR=1
    )
    target_include_directories(fmt
      INTERFACE
        "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/fmt/include>"
    )
  endif()

  add_library(fmt::fmt ALIAS fmt)
  add_library(fmt::fmt-header-only ALIAS fmt)
endif()

set(OSSIA_HAS_FMT 1 CACHE INTERNAL "")
