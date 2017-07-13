#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <chobo/small_vector.hpp>
namespace ossia
{
template <typename T>
using value_vector = chobo::small_vector<T, 4>;

using audio_vector = std::array<double, 64>;

struct audio_port
{
  value_vector<audio_vector> samples;
};

struct midi_port
{
  value_vector<mm::MidiMessage> messages;
};

struct value_port
{
  value_vector<ossia::value> data;
};

struct clear_data
{
  void operator()(value_port& p) const
  {
    p.data.clear();
  }

  void operator()(midi_port& p) const
  {
    p.messages.clear();
  }

  void operator()(audio_port& p) const
  {
    p.samples.clear();
  }

  void operator()() const
  {
  }
};

struct data_size
{
  std::size_t operator()(const value_port& p) const
  {
    return p.data.size();
  }

  std::size_t operator()(const midi_port& p) const
  {
    return p.messages.size();
  }

  std::size_t operator()(const audio_port& p) const
  {
    return p.samples.size();
  }

  std::size_t operator()() const
  {
    return 0;
  }
};

struct copy_data
{
  template <typename T, typename U>
  void operator()(const T&, const U&) const
  {
  }

  void operator()(const value_port& out, value_port& in)
  {
    for (const auto& v : out.data)
      in.data.push_back(v);
  }

  void operator()(const audio_port& out, audio_port& in)
  {
    in.samples.reserve(in.samples.size() + out.samples.size());
    for (const auto& s : out.samples)
      in.samples.push_back(s);
  }

  void operator()(const midi_port& out, midi_port& in)
  {
    for (const auto& v : out.messages)
      in.messages.push_back(v);
  }
};

struct copy_data_pos
{
  const std::size_t pos;

  template <typename T, typename U>
  void operator()(const T&, const U&) const
  {
  }

  void operator()(const value_port& out, value_port& in)
  {
    if (pos < out.data.size())
      in.data.push_back(out.data[pos]);
  }

  void operator()(const audio_port& out, audio_port& in)
  {
    if (pos < out.samples.size())
      in.samples.push_back(out.samples[pos]);
  }

  void operator()(const midi_port& out, midi_port& in)
  {
    if (pos < out.messages.size())
      in.messages.push_back(out.messages[pos]);
  }
};
}
