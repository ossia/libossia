# Steinberg ASIO SDK (submodule at 3rdparty/asio)

set(ASIO_SDK_DIR "${OSSIA_3RDPARTY_FOLDER}/asio")

if(EXISTS "${ASIO_SDK_DIR}/common/asio.h" AND WIN32)
  add_library(asio_sdk STATIC)
  add_library(asio::sdk ALIAS asio_sdk)

  # Must not be built with UNICODE/_UNICODE: asiolist.cpp is ANSI-only and
  # enumerates zero drivers if the TCHAR registry calls resolve to -W.

  # Upstream iasiodrv.h has no include guard and all three sources include it:
  # in a unity build they land in one translation unit and IASIO is defined
  # three times.
  set_target_properties(asio_sdk PROPERTIES UNITY_BUILD OFF)

  # The SDK is written against the unabridged <windows.h>. iasiodrv.h declares
  # IASIO with the `interface` keyword, which is a macro that <windows.h> only
  # reaches through <ole2.h>, and WIN32_LEAN_AND_MEAN is precisely the switch
  # that cuts <ole2.h> out - so every translation unit here stops compiling with
  # "unknown type name 'interface'" as soon as an embedder defines it globally.
  # score does, for all C++ (see its ScoreConfiguration.cmake), which is why
  # this cannot be left to the consumer: libossia's own build never defines it,
  # so the breakage only appears once libossia is built inside score.
  #
  # Undefining it for this target is not reliable: Ninja and the Makefiles emit
  # definitions before the compile options, but the Visual Studio generators put
  # AdditionalOptions before PreprocessorDefinitions, so there the -D wins back
  # over the -U. Instead, force-include the headers iasiodrv.h is missing in
  # front of every source; <unknwn.h> pulls <ole2.h> in whatever the define.
  if(MSVC)
    target_compile_options(asio_sdk PRIVATE "/FI${CMAKE_CURRENT_LIST_DIR}/asio-sdk-prelude.h")
  else()
    target_compile_options(asio_sdk PRIVATE "-include" "${CMAKE_CURRENT_LIST_DIR}/asio-sdk-prelude.h")
  endif()

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
