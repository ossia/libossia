#pragma once
#include <cstdint>

namespace ossia
{
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
