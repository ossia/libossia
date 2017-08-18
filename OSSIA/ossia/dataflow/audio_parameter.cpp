#include "audio_parameter.hpp"

namespace ossia
{

audio_address::audio_address(net::node_base& n)
  : m_node{n}
{
}

void audio_address::clone_value(audio_vector& res_vec) const
{
  if(res_vec.size() < audio.size())
    res_vec.resize(audio.size());

  for(std::size_t chan = 0; chan < res_vec.size(); chan++)
  {
    auto& src = audio[chan];
    auto& res = res_vec[chan];
    const auto N = std::min((int)src.size(), (int)res.size());
    for (int i = 0; i < N; i++)
      res[i] += src[i];
  }
}

void audio_address::push_value(const audio_port& port)
{
  auto min_chan = std::min(port.samples.size(), (std::size_t)audio.size());
  for(std::size_t chan = 0; chan < min_chan; chan++)
  {
    auto& src = port.samples[chan];
    auto& dst = audio[chan];
    std::copy_n(
          src.begin(),
          std::min(src.size(), (std::size_t)dst.size()),
          dst.begin());
  }
}

net::node_base&audio_address::get_node() const
{
  return m_node;
}

void audio_address::pull_value()
{
}

ossia::net::parameter_base& audio_address::push_value(const ossia::value&)
{
  return *this;
}

ossia::net::parameter_base& audio_address::push_value(ossia::value&&)
{
  return *this;
}

net::parameter_base&audio_address::push_value()
{
  return *this;
}

value audio_address::value() const
{
  return {};
}

net::parameter_base&audio_address::set_value(const ossia::value&)
{
  return *this;
}

net::parameter_base&audio_address::set_value(ossia::value&&)
{
  return *this;
}

val_type audio_address::get_value_type() const
{
  return {};
}

net::parameter_base&audio_address::set_value_type(val_type)
{
  return *this;
}

access_mode audio_address::get_access() const
{
  return {};
}

net::parameter_base&audio_address::set_access(access_mode)
{
  return *this;
}

const domain&audio_address::get_domain() const
{
  static ossia::domain d;
  return d;
}

net::parameter_base&audio_address::set_domain(const domain&)
{
  return *this;
}

bounding_mode audio_address::get_bounding() const
{
  return {};
}

net::parameter_base&audio_address::set_bounding(bounding_mode)
{
  return *this;
}

repetition_filter audio_address::get_repetition_filter() const
{
  return {};
}

net::parameter_base&audio_address::set_repetition_filter(repetition_filter)
{
  return *this;
}

}
