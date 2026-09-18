#pragma once
#include <ossia/detail/config.hpp>

#include <cstdint>

namespace ossia
{
//! Whether the execution graph can convert a sample rate at all. When it
//! cannot, material has to reach it at the graph's own rate.
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
