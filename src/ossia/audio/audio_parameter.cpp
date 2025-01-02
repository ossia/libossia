#include "audio_parameter.hpp"

#include "audio_protocol.hpp"

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/network/common/complex_type.hpp>

namespace ossia
{

audio_parameter::audio_parameter(net::node_base& n)
    : ossia::net::parameter_base{n}
{
  m_type = parameter_type::AUDIO;
}

audio_parameter::~audio_parameter() = default;

void audio_parameter::clone_value(audio_vector& res_vec) const
{
  {
    int orig_dst_size = res_vec.size();
    if(orig_dst_size < audio.size())
    {
      ossia::audio_buffer_pool::set_channels(res_vec, audio.size());

      for(std::size_t chan = orig_dst_size; chan < res_vec.size(); chan++)
      {
        res_vec[chan].clear();
      }
    }
  }

  auto min_chan = std::min(res_vec.size(), audio.size());
  for(std::size_t chan = 0; chan < min_chan; chan++)
  {
    auto& src = audio[chan];
    auto& res = res_vec[chan];

    const std::size_t N = src.size();

    if(res.size() < N)
      res.resize(N);

    for(std::size_t i = 0; i < N; i++)
      res[i] += double(src[i]);
  }
}

void audio_parameter::push_value(const audio_port& port)
{
  auto min_chan = std::min(port.channels(), (std::size_t)audio.size());
  for(std::size_t chan = 0; chan < min_chan; chan++)
  {
    auto& src = port.channel(chan);
    auto& dst = audio[chan];
    const auto N = std::min(src.size(), (std::size_t)dst.size());
    for(std::size_t i = 0; i < N; i++)
    {
      // Important: here we must not mix
      dst[i] = float(src[i] * m_gain);
    }
  }
}

void audio_parameter::pull_value() { }

ossia::net::parameter_base& audio_parameter::push_value(const ossia::value& v)
{
  set_value(v);
  return *this;
}

ossia::net::parameter_base& audio_parameter::push_value(ossia::value&& v)
{
  set_value(v);
  return *this;
}

net::parameter_base& audio_parameter::push_value()
{
  return *this;
}

value audio_parameter::value() const
{
  return m_gain;
}

ossia::value audio_parameter::set_value(const ossia::value& v)
{
  auto flt = ossia::convert<float>(v);
  auto vol = ossia::clamp(flt, 0.f, 1.f);
  if(m_gain != vol)
  {
    m_gain = vol;
    send(vol);
  }
  return m_gain;
}

ossia::value audio_parameter::set_value(ossia::value&& v)
{
  return set_value(v);
}

val_type audio_parameter::get_value_type() const noexcept
{
  return ossia::val_type::FLOAT;
}

net::parameter_base& audio_parameter::set_value_type(val_type)
{
  return *this;
}

access_mode audio_parameter::get_access() const noexcept
{
  return {};
}

net::parameter_base& audio_parameter::set_access(access_mode)
{
  return *this;
}

const domain& audio_parameter::get_domain() const noexcept
{
  static ossia::domain d;
  return d;
}

net::parameter_base& audio_parameter::set_domain(const domain&)
{
  return *this;
}

bounding_mode audio_parameter::get_bounding() const noexcept
{
  return {};
}

net::parameter_base& audio_parameter::set_bounding(bounding_mode)
{
  return *this;
}

virtual_audio_parameter::virtual_audio_parameter(int num_channels, net::node_base& n)
    : audio_parameter{n}
    , m_audio_data(num_channels)
{
  set_buffer_size(512);
  auto& proto = static_cast<ossia::audio_protocol&>(n.get_device().get_protocol());
  proto.register_parameter(*this);
}

virtual_audio_parameter::~virtual_audio_parameter()
{
  auto& proto
      = static_cast<ossia::audio_protocol&>(get_node().get_device().get_protocol());
  proto.unregister_parameter(*this);
}

mapped_audio_parameter::mapped_audio_parameter(
    bool output, audio_mapping m, net::node_base& n)
    : audio_parameter{n}
    , mapping(std::move(m))
    , is_output{output}
{
  auto& proto = static_cast<ossia::audio_protocol&>(n.get_device().get_protocol());
  proto.register_parameter(*this);
}

mapped_audio_parameter::~mapped_audio_parameter()
{
  auto& proto
      = static_cast<ossia::audio_protocol&>(get_node().get_device().get_protocol());
  proto.unregister_parameter(*this);
}
}
