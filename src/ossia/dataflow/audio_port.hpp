#pragma once
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/math.hpp>
#include <vector>
#include <ossia/detail/config.hpp>
namespace ossia
{

OSSIA_EXPORT
void ensure_vector_sizes(const audio_vector& src_vec, audio_vector& sink_vec);

using pan_weight = ossia::small_vector<double, 2>;
struct inlet;
struct audio_port
{
  static const constexpr int which = 0;

  audio_vector samples;

  operator ossia::mutable_audio_span<double>() noexcept
  {
    return {samples.begin(), samples.end()};
  }

  operator ossia::audio_span<double>() const noexcept
  {
    return {samples.begin(), samples.end()};
  }
};

#if BOOST_VERSION >= 107200
static_assert(noexcept(audio_port{}));
#endif

struct audio_delay_line
{
  std::vector<audio_vector> samples;
};
}
