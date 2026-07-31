# Steinberg ASIO SDK (submodule at 3rdparty/asio)

set(ASIO_SDK_DIR "${OSSIA_3RDPARTY_FOLDER}/asio")

if(EXISTS "${ASIO_SDK_DIR}/common/asio.h" AND WIN32)
  add_library(asio_sdk STATIC)
  add_library(asio::sdk ALIAS asio_sdk)

  # Do NOT define UNICODE/_UNICODE here. host/pc/asiolist.cpp is an ANSI-only
  # translation unit: it parses the registry into char buffers using explicit
  # -A calls (RegOpenKeyA, RegQueryValueExA, CharLowerBuffA, strcmp) but reads
  # key names with the TCHAR-generic RegEnumKey/RegOpenKeyEx. With UNICODE
  # defined those resolve to the -W variants, which write UTF-16 into those
  # char buffers. findDrvPath() then compares a UTF-16 CLSID against an ANSI
  # one with strcmp(), which can never match (strlen() stops at the first
  # embedded NUL), so every driver is rejected and asioGetNumDev() returns 0 --
  # the device list silently comes up empty. It also overflows the buffers,
  # since the size arguments are character counts. See #1422.

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
