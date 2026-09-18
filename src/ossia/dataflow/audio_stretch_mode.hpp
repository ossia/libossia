#pragma once
#include <ossia/detail/config.hpp>

#include <cstdint>

namespace ossia
{
//! Without it, material has to reach the graph at the graph's own rate.
#if defined(OSSIA_ENABLE_LIBSAMPLERATE)
inline constexpr bool graph_resampling = true;
#else
inline constexpr bool graph_resampling = false;
#endif

enum audio_stretch_mode : int8_t
{
  None,
  RubberBandStandard,
  RubberBandPercussive,
  Repitch,
  RubberBandStandardHQ,
  RubberBandPercussiveHQ,
  RepitchMediumQ,
  RepitchFastestQ,
};
}
