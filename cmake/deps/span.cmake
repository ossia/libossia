if(OSSIA_USE_SYSTEM_LIBRARIES)
  find_package(span CONFIG GLOBAL)
endif()

# NB: the backing target is named "ossia_span" rather than "span" to avoid a
# clash with Abseil, which registers an un-prefixed "span" target (alias
# absl::span) when ABSL_ENABLE_INSTALL is ON (see deps/abseil.cmake). Consumers
# use the span::span alias.
if(NOT TARGET span::span)
  add_library(ossia_span INTERFACE IMPORTED GLOBAL)
  add_library(span::span ALIAS ossia_span)
  target_include_directories(ossia_span INTERFACE "$<BUILD_INTERFACE:${OSSIA_3RDPARTY_FOLDER}/span/include>")
endif()
