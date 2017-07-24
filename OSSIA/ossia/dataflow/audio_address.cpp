#include "audio_address.hpp"

namespace ossia
{

audio_address::audio_address(net::node_base& n)
  : m_node{n}
{
}

void audio_address::clone_value(audio_vector& res) const
{
  const auto N = std::min((int)audio.size(), (int)res.size());
  for (int i = 0; i < N; i++)
    res[i] += audio[i];
}

void audio_address::push_value(const audio_port& port)
{
  std::copy_n(
        port.samples.begin(),
        std::min((std::size_t)audio.size(), (std::size_t)port.samples.size()),
        audio.begin());
}

net::node_base&audio_address::get_node() const
{
  return m_node;
}

void audio_address::pull_value()
{
}

ossia::net::address_base& audio_address::push_value(const ossia::value&)
{
  return *this;
}

ossia::net::address_base& audio_address::push_value(ossia::value&&)
{
  return *this;
}

net::address_base&audio_address::push_value()
{
  return *this;
}

value audio_address::value() const
{
  return {};
}

net::address_base&audio_address::set_value(const ossia::value&)
{
  return *this;
}

net::address_base&audio_address::set_value(ossia::value&&)
{
  return *this;
}

val_type audio_address::get_value_type() const
{
  return {};
}

net::address_base&audio_address::set_value_type(val_type)
{
  return *this;
}

access_mode audio_address::get_access() const
{
  return {};
}

net::address_base&audio_address::set_access(access_mode)
{
  return *this;
}

const domain&audio_address::get_domain() const
{
  static ossia::domain d;
  return d;
}

net::address_base&audio_address::set_domain(const domain&)
{
  return *this;
}

bounding_mode audio_address::get_bounding() const
{
  return {};
}

net::address_base&audio_address::set_bounding(bounding_mode)
{
  return *this;
}

repetition_filter audio_address::get_repetition_filter() const
{
  return {};
}

net::address_base&audio_address::set_repetition_filter(repetition_filter)
{
  return *this;
}

}
