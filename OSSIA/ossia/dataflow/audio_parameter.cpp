#include "audio_parameter.hpp"

namespace ossia
{

audio_parameter::audio_parameter(net::node_base& n)
  : ossia::net::parameter_base{n}
{
}

void audio_parameter::clone_value(audio_vector& res_vec) const
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

void audio_parameter::push_value(const audio_port& port)
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

void audio_parameter::pull_value()
{
}

ossia::net::parameter_base& audio_parameter::push_value(const ossia::value&)
{
  return *this;
}

ossia::net::parameter_base& audio_parameter::push_value(ossia::value&&)
{
  return *this;
}

net::parameter_base&audio_parameter::push_value()
{
  return *this;
}

value audio_parameter::value() const
{
  return {};
}

net::parameter_base&audio_parameter::set_value(const ossia::value&)
{
  return *this;
}

net::parameter_base&audio_parameter::set_value(ossia::value&&)
{
  return *this;
}

val_type audio_parameter::get_value_type() const
{
  return {};
}

net::parameter_base&audio_parameter::set_value_type(val_type)
{
  return *this;
}

access_mode audio_parameter::get_access() const
{
  return {};
}

net::parameter_base&audio_parameter::set_access(access_mode)
{
  return *this;
}

const domain&audio_parameter::get_domain() const
{
  static ossia::domain d;
  return d;
}

net::parameter_base&audio_parameter::set_domain(const domain&)
{
  return *this;
}

bounding_mode audio_parameter::get_bounding() const
{
  return {};
}

net::parameter_base&audio_parameter::set_bounding(bounding_mode)
{
  return *this;
}


}
