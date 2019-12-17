#pragma once
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/math.hpp>
#include <vector>
#include <ossia_export.h>
namespace ossia
{
using pan_weight = ossia::small_vector<double, 2>;
struct inlet;
struct audio_port
{
  static const constexpr int which = 0;

  audio_vector samples;

  double gain{1.};
  pan_weight pan{ossia::sqrt_2 / 2., ossia::sqrt_2 / 2.};
  bool upmix{};
  bool has_gain{};
};

struct audio_delay_line
{
  std::vector<audio_vector> samples;
};
}
