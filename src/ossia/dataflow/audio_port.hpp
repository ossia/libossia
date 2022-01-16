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

  static void set_channels(audio_vector& samples, std::size_t channels);
};

using pan_weight = ossia::small_vector<double, 2>;
struct inlet;
struct audio_port
{
  static const constexpr int which = 0;

  audio_port() noexcept
  {
    set_channels(2);
  }

  audio_port(const audio_port& other) noexcept
  {
    *this = other;
  }

  audio_port(audio_port&& other) noexcept
    : m_samples{std::move(other.m_samples)}
  {
  }

  audio_port& operator=(const audio_port& other) noexcept
  {
    audio_buffer_pool::set_channels(m_samples, other.channels());
    for(std::size_t c = 0; c < other.channels(); c++)
    {
      channel(c) = other.channel(c);
    }
    return *this;
  }

  audio_port& operator=(audio_port&& other) noexcept
  {
    m_samples = std::move(other.m_samples);
    other.set_channels(2);

    return *this;
  }

  audio_channel& channel(std::size_t i) noexcept
  { return m_samples[i]; }

  const audio_channel& channel(std::size_t i) const noexcept
  { return m_samples[i]; }

  std::size_t channels() const noexcept
  { return m_samples.size(); }

  bool empty() const noexcept
  { return m_samples.empty(); }

  void set_channels(std::size_t channels)
  {
    return audio_buffer_pool::set_channels(m_samples, channels);
  }

  operator ossia::mutable_audio_span<double>() noexcept
  {
    return {m_samples.begin(), m_samples.end()};
  }

  operator ossia::audio_span<double>() const noexcept
  {
    return {m_samples.begin(), m_samples.end()};
  }

  audio_vector& get() noexcept { return m_samples; }
  const audio_vector& get() const noexcept { return m_samples; }

  auto begin() const noexcept { return m_samples.begin(); }
  auto end() const noexcept { return m_samples.end(); }
  auto cbegin() const noexcept { return m_samples.cbegin(); }
  auto cend() const noexcept { return m_samples.cend(); }
  auto rbegin() const noexcept { return m_samples.rbegin(); }
  auto rend() const noexcept { return m_samples.rend(); }
  auto crbegin() const noexcept { return m_samples.crbegin(); }
  auto crend() const noexcept { return m_samples.crend(); }
  auto begin() noexcept { return m_samples.begin(); }
  auto end() noexcept { return m_samples.end(); }
  auto cbegin() noexcept { return m_samples.cbegin(); }
  auto cend() noexcept { return m_samples.cend(); }
  auto rbegin() noexcept { return m_samples.rbegin(); }
  auto rend() noexcept { return m_samples.rend(); }
  auto crbegin() noexcept { return m_samples.crbegin(); }
  auto crend() noexcept { return m_samples.crend(); }
private:
  friend void ensure_vector_sizes(const audio_vector& src_vec, audio_vector& sink_vec);
  audio_vector m_samples;
};

#if BOOST_VERSION >= 107200
static_assert(noexcept(audio_port{}));
#endif

struct audio_delay_line
{
  std::vector<audio_vector> samples;
};

}
