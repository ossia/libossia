#pragma once
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/small_vector.hpp>
#include <vector>
#include <ossia_export.h>
namespace ossia
{
using pan_weight = ossia::small_vector<double, 2>;
struct audio_port
{
  audio_vector samples;
  double gain{1.};
  pan_weight pan{};
  bool upmix{};
  bool has_gain{};
};

struct audio_delay_line
{
  std::vector<audio_vector> samples;
};
}
