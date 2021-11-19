#pragma once
#include <ossia/dataflow/nodes/media.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/math.hpp>
#include <ossia/detail/buffer_pool.hpp>
#include <vector>
#include <ossia/detail/config.hpp>
namespace ossia
{

OSSIA_EXPORT
void ensure_vector_sizes(const audio_vector& src_vec, audio_vector& sink_vec);

OSSIA_EXPORT
void mix(const audio_vector& src_vec, audio_vector& sink_vec);

struct OSSIA_EXPORT audio_buffer_pool
    : object_pool<audio_channel>
{
  audio_buffer_pool();
  ~audio_buffer_pool();
  static audio_buffer_pool& instance() noexcept;

  static void set_channels(audio_vector& samples, std::size_t channels)
  {
    if(samples.size() == channels)
      return;

    auto& pool = audio_buffer_pool::instance();
    while(samples.size() > channels)
    {
      auto chan = std::move(samples.back());
      chan.clear();
      pool.release(std::move(chan));
      samples.pop_back();
    }

    while(samples.size() < channels)
    {
      samples.push_back(pool.acquire());
    }
  }
};

using pan_weight = ossia::small_vector<double, 2>;
struct inlet;
struct audio_port
{
  static const constexpr int which = 0;

  audio_vector samples;

  audio_port() noexcept
  {
    set_channels(2);
  }
  audio_port(const audio_port& other) noexcept
    : samples{other.samples}
  {
  }
  audio_port(audio_port&& other) noexcept
    : samples{std::move(other.samples)}
  {
  }
  audio_port& operator=(const audio_port& other) noexcept
  {
    samples = other.samples;
    return *this;
  }
  audio_port& operator=(audio_port&& other) noexcept
  {
    samples = std::move(other.samples);
    other.set_channels(2);

    return *this;
  }

  std::size_t channels() const noexcept
  { return samples.size(); }

  void set_channels(std::size_t channels)
  {
    return audio_buffer_pool::set_channels(samples, channels);
  }

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
