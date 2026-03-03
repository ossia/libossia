# Steinberg ASIO SDK (submodule at 3rdparty/asio)

set(ASIO_SDK_DIR "${OSSIA_3RDPARTY_FOLDER}/asio")

if(EXISTS "${ASIO_SDK_DIR}/common/asio.h" AND WIN32)
  add_library(asio_sdk STATIC)
  add_library(asio::sdk ALIAS asio_sdk)
  target_compile_definitions(asio_sdk PRIVATE UNICODE=1 _UNICODE=1)

  target_include_directories(asio_sdk SYSTEM PUBLIC
    $<BUILD_INTERFACE:${ASIO_SDK_DIR}/common>
    $<BUILD_INTERFACE:${ASIO_SDK_DIR}/host>
    $<BUILD_INTERFACE:${ASIO_SDK_DIR}/host/pc>
  )

  # ASIO SDK requires these source files to be compiled into the host application
  target_sources(asio_sdk PRIVATE
    ${ASIO_SDK_DIR}/common/asio.cpp
    ${ASIO_SDK_DIR}/host/asiodrivers.cpp
    ${ASIO_SDK_DIR}/host/pc/asiolist.cpp
  )

  # ASIO SDK requires COM and advapi32 on Windows
  target_link_libraries(asio_sdk PRIVATE ole32 advapi32)

  # OSSIA_ENABLE_ASIO is set via ossia-config.hpp.in / #cmakedefine
endif()
