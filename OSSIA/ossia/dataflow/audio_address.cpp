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
    res[i] = audio[i];

  // Fill remaining with zeros.
  const int M = res.size();
  for (int i = N; i < M; i++)
    res[i] = 0.;
}

void audio_address::push_value(const audio_port& port)
{
  const int port_N = port.samples.size();

  // The first buffer, if it exists, will fill with zeros
  if (port_N > 0)
  {
    const int N = std::min((int)audio.size(), (int)port.samples[0].size());

    // First buffer
    for (int j = 0; j < N; j++)
    {
      audio[j] = port.samples[0][j];
    }
    // Following buffers are mixed
    for (int i = 1; i < port_N; i++)
    {
      auto& v = port.samples[i];
      const int N = std::min((int)audio.size(), (int)v.size());

      for (int j = 0; j < N; j++)
        audio[j] += v[j];
    }
  }
  else
  {
    for (auto& v : audio)
      v = 0.;
  }
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
