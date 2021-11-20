#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
namespace ossia
{

struct clear_data
{
  void operator()(value_port& p) const
  {
    p.clear();
  }

  void operator()(midi_port& p) const
  {
    p.messages.clear();
  }

  void operator()(audio_port& p) const
  {
    p.set_channels(0);
  }

  void operator()() const
  {
  }
};

struct data_size
{
  std::size_t operator()(const value_delay_line& p) const
  {
    return p.data.size();
  }

  std::size_t operator()(const midi_delay_line& p) const
  {
    return p.messages.size();
  }

  std::size_t operator()(const audio_delay_line& p) const
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
  /// Value ///
  void operator()(
      const value_vector<std::pair<ossia::typed_value, int>>& out,
      value_port& in)
  {
    // Called in local_pull_visitor
    for (auto& val : out)
      in.add_local_value(val.first);
  }
  void operator()(const value_vector<ossia::typed_value>& out, value_port& in)
  {
    // Called in copy_data_pos, when copying from a delay line to a port
    for (auto& val : out)
      in.add_local_value(val);
  }

  void operator()(const value_port& out, value_port& in)
  {
    // Called in init_node_visitor::copy, when copying from a node to another
    in.add_port_values(out);
  }

  void operator()(const net::parameter_base& param, value_port& in)
  {
    // Called from global_pull_visitor
    in.add_global_value(param, param.value());
  }

  void operator()(
      const net::parameter_base& param, const value_vector<ossia::value>& vec,
      value_port& in)
  {
    // Called from global_pull_visitor
    in.add_global_values(param, vec);
  }

  void operator()(const value_port& out, value_delay_line& in)
  {
    // Called in env_writer, when copying from a node to a delay line
    value_vector<ossia::typed_value> vec;
    vec.reserve(out.get_data().size());
    for (const ossia::timed_value& val : out.get_data())
    {
      vec.emplace_back(val, out.index, out.type);
    }
    in.data.push_back(std::move(vec));
  }

  /// Audio ///
  void operator()(const audio_port& out, audio_delay_line& in)
  {
    // Called in env_writer, when copying from a node to a delay line
    in.samples.push_back(out.get());
  }

  void operator()(const audio_port& out, audio_port& in)
  {
    // Called in init_node_visitor::copy, when copying from a node to another
    mix(out.get(), in.get());
  }

  /// MIDI ///
  void operator()(const midi_port& out, midi_port& in)
  {
    // Called in init_node_visitor::copy, when copying from a node to another
    operator()(out.messages, in);
  }

  void operator()(const value_vector<libremidi::message>& out, midi_port& in)
  {
    // Called in copy_data_pos, when copying from a delay line to a port
    for (const auto& data : out)
      in.messages.push_back(data);
  }

  void operator()(const midi_port& out, midi_delay_line& in)
  {
    // Called in env_writer, when copying from a node to a delay line
    in.messages.push_back(out.messages);
  }
};

struct copy_data_pos
{
  const std::size_t pos;

  template <typename T, typename U>
  void operator()(const T&, const U&) const
  {
  }

  void operator()(const value_delay_line& out, value_port& in)
  {
    if (pos < out.data.size())
    {
      copy_data{}(out.data[pos], in);
    }
  }

  void operator()(const audio_delay_line& out, audio_port& in)
  {
    if (pos < out.samples.size())
    {
      mix(out.samples[pos], in.get());
    }
  }

  void operator()(const midi_delay_line& out, midi_port& in)
  {
    if (pos < out.messages.size())
    {
      copy_data{}(out.messages[pos], in);
    }
  }
};
}
