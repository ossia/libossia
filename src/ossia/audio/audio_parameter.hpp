#pragma once

#include <ossia/dataflow/audio_port.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/pod_vector.hpp>
#include <span>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/value/value.hpp>

namespace ossia
{
class OSSIA_EXPORT audio_parameter : public ossia::net::parameter_base
{

public:
  ossia::small_vector<std::span<float>, 8> audio;
  double m_gain{1.};

  explicit audio_parameter(ossia::net::node_base& n);

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
};

class OSSIA_EXPORT virtual_audio_parameter final : public audio_parameter
{
  // todo use a flat vector instead for perf
  std::vector<ossia::float_vector> m_audio_data;

public:
  virtual_audio_parameter(int num_channels, ossia::net::node_base& n);

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
