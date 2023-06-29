find_path(PIPEWIRE_INCLUDEDIR pipewire-0.3/pipewire/filter.h)
find_path(SPA_INCLUDEDIR spa-0.2/spa/param/latency-utils.h)

if(PIPEWIRE_INCLUDEDIR AND SPA_INCLUDEDIR)
  set(PIPEWIRE_INCLUDEDIR "${PIPEWIRE_INCLUDEDIR}/pipewire-0.3")
  set(SPA_INCLUDEDIR "${SPA_INCLUDEDIR}/spa-0.2")

  add_library(pipewire IMPORTED INTERFACE GLOBAL)
  add_library(pipewire::pipewire ALIAS pipewire)
  target_include_directories(pipewire INTERFACE "${PIPEWIRE_INCLUDEDIR}" "${SPA_INCLUDEDIR}")
endif()

