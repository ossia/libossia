#pragma once
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/small_vector.hpp>
#include <vector>
#include <ossia_export.h>
namespace ossia
{
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
