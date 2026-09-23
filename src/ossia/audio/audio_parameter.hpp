#pragma once

#include <ossia/dataflow/audio_port.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <span>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>

#include <atomic>

namespace ossia::telemetry
{
struct meter_tap;
}

namespace ossia
{
class OSSIA_EXPORT audio_parameter : public ossia::net::parameter_base
{

public:
  ossia::small_vector<std::span<float>, 8> audio;

  //! Where gain() is applied, so that it is applied exactly once.
  enum class gain_stage : uint8_t
  {
    push,     //!< on what the graph writes (outputs, virtual ports)
    pull,     //!< on what the graph reads (inputs)
    external, //!< by whoever owns the buffers, after the sum (/out/main)
  } stage{gain_stage::push};

  //! A parameter whose gain also applies to this one: /in/main for the
  //! inputs.
  const audio_parameter* upstream{};

  explicit audio_parameter(ossia::net::node_base& n);

  //! Linear gain, readable from any thread.
  [[nodiscard]] float gain() const noexcept
  {
    return m_gain.load(std::memory_order_relaxed);
  }

  virtual ~audio_parameter();

  void clone_value(audio_vector& res) const;
  virtual void push_value(const audio_port& port);

  void pull_value() override;
  net::parameter_base& push_value(const ossia::value&) override;
  net::parameter_base& push_value(ossia::value&&) override;
  net::parameter_base& push_value() override;
  ossia::value value() const override;
  ossia::value set_value(const ossia::value&) override;
  ossia::value set_value(ossia::value&&) override;
  val_type get_value_type() const noexcept override;
  net::parameter_base& set_value_type(val_type) override;
  access_mode get_access() const noexcept override;
  net::parameter_base& set_access(access_mode) override;
  const domain& get_domain() const noexcept override;
  net::parameter_base& set_domain(const domain&) override;
  bounding_mode get_bounding() const noexcept override;
  net::parameter_base& set_bounding(bounding_mode) override;

protected:
  std::atomic<float> m_gain{1.f};
};

class OSSIA_EXPORT virtual_audio_parameter final : public audio_parameter
{
  // todo use a flat vector instead for perf
  std::vector<ossia::float_vector> m_audio_data;

public:
  virtual_audio_parameter(int num_channels, ossia::net::node_base& n);

  //! Buffers for another channel count, made off the audio thread.
  struct channels
  {
    std::vector<ossia::float_vector> data;
    ossia::small_vector<std::span<float>, 8> spans;
  };
  [[nodiscard]] channels make_channels(int num_channels) const;
  //! Takes buffers made by make_channels, between two ticks; `c` gets the
  //! previous ones, to be freed off the audio thread.
  void swap_channels(channels& c) noexcept
  {
    std::swap(m_audio_data, c.data);
    std::swap(audio, c.spans);
  }

  //! Changes the channel count; only while the audio callback does not run.
  void set_channels(int num_channels)
  {
    const auto bs = m_audio_data.empty() ? 0 : m_audio_data.front().size();
    m_audio_data.resize(std::max(num_channels, 0));
    set_buffer_size(int(bs));
  }

  void set_buffer_size(int bs)
  {
    const auto chan = m_audio_data.size();
    audio.resize(chan);
    for(std::size_t i = 0; i < chan; i++)
    {
      m_audio_data[i].resize(bs);
      audio[i] = m_audio_data[i];
    }
  }

  void push_value(const audio_port& port) override;

  //! Meters what the graph writes here. Set by the interface thread; the
  //! telemetry arena owns the tap and keeps it alive until the audio thread
  //! can no longer be writing to it.
  std::atomic<ossia::telemetry::meter_tap*> meter{};

  virtual ~virtual_audio_parameter();
};

using audio_mapping = ossia::int_vector;
class OSSIA_EXPORT mapped_audio_parameter final : public audio_parameter
{
public:
  audio_mapping mapping;
  bool is_output{false};
  mapped_audio_parameter(bool output, audio_mapping m, ossia::net::node_base& n);

  virtual ~mapped_audio_parameter();
};
}
