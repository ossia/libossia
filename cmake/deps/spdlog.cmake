if(OSSIA_USE_SYSTEM_LIBRARIES)
  if(NOT OSSIA_FMT_INTERNAL)
    find_package(spdlog CONFIG REQUIRED GLOBAL)
  endif()
endif()

if(NOT TARGET spdlog::spdlog)
  if(OSSIA_FAST_DEVELOPER_BUILD)
    add_library(spdlog SHARED
       ${OSSIA_3RDPARTY_FOLDER}/spdlog/src/spdlog.cpp
       ${OSSIA_3RDPARTY_FOLDER}/spdlog/src/stdout_sinks.cpp
       ${OSSIA_3RDPARTY_FOLDER}/spdlog/src/color_sinks.cpp
       ${OSSIA_3RDPARTY_FOLDER}/spdlog/src/file_sinks.cpp
       ${OSSIA_3RDPARTY_FOLDER}/spdlog/src/async.cpp
       ${OSSIA_3RDPARTY_FOLDER}/spdlog/src/cfg.cpp
    )
    target_compile_definitions(spdlog
      PUBLIC
        SPDLOG_COMPILED_LIB=1
        SPDLOG_SHARED_LIB=1
        SPDLOG_FMT_EXTERNAL=1

        SPDLOG_NO_DATETIME=1
        SPDLOG_NO_THREAD_ID=1
        SPDLOG_NO_NAME=1

        SPDLOG_DEBUG_ON=1
        SPDLOG_TRACE_ON=1
    )
    target_include_directories(spdlog
      PUBLIC
        "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/spdlog/include>"
    )
    target_link_libraries(spdlog
      PUBLIC
        "$<BUILD_INTERFACE:fmt::fmt>"
    )
  else()
    add_library(spdlog INTERFACE IMPORTED GLOBAL)
    target_compile_definitions(spdlog
      INTERFACE
        SPDLOG_FMT_EXTERNAL=1

        SPDLOG_NO_DATETIME=1
        SPDLOG_NO_THREAD_ID=1
        SPDLOG_NO_NAME=1

        SPDLOG_DEBUG_ON=1
        SPDLOG_TRACE_ON=1
    )
    target_include_directories(spdlog
      INTERFACE
        "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/spdlog/include>"
    )
    target_link_libraries(spdlog INTERFACE fmt::fmt)
  endif()

  add_library(spdlog::spdlog ALIAS spdlog)
endif()
