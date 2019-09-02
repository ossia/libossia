#pragma once
#include <ossia/detail/small_vector.hpp>
#include <vector>
namespace ossia
{
using audio_channel = ossia::small_vector<double, 256>;
using audio_vector = ossia::small_vector<audio_channel, 2>;

struct audio_port
{
  bool upmix{};
  audio_vector samples;
};

struct audio_delay_line
{
  std::vector<audio_vector> samples;
};
}
