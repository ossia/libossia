#pragma once
#include <cinttypes>

namespace ossia
{
struct audio_tick_state
{
  float* const* inputs{};
  float** outputs{};
  int32_t n_in{};
  int32_t n_out{};
  uint64_t frames{};
  double seconds{};
};
}
