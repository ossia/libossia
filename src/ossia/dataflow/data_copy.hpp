#pragma once
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/data.hpp>
#include <ossia/dataflow/geometry_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#include <ossia/dataflow/typed_value.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/nullable_variant.hpp>
#include <ossia/network/base/parameter.hpp>
namespace ossia
{

struct clear_data
{
  void operator()(value_port& p) const { p.clear(); }

  void operator()(midi_port& p) const { p.messages.clear(); }

  void operator()(audio_port& p) const { p.set_channels(0); }

  void operator()(geometry_port& p) const { p.clear(); }

  void operator()() const { }
};

struct data_size
{
  std::size_t operator()(const value_delay_line& p) const { return p.data.size(); }

  std::size_t operator()(const midi_delay_line& p) const { return p.messages.size(); }

  std::size_t operator()(const audio_delay_line& p) const { return p.samples.size(); }

  std::size_t operator()(const geometry_delay_line& p) const
  {
    return p.geometries.size();
  }

  std::size_t operator()(const ossia::monostate&) const { return 0; }
  std::size_t operator()() const { return 0; }
};

struct move_data
{
  void operator()(value_port& out, value_port& in)
  {
    in.add_port_values(std::move(out));
  }

  void operator()(audio_port& out, audio_port& in)
  {
    auto tmp = std::move(in.get());
    in.get() = std::move(out.get());
    out.get() = std::move(tmp);
  }

  void operator()(midi_port& out, midi_port& in)
  {
    auto tmp = std::move(in.messages);
    in.messages = std::move(out.messages);
    out.messages = std::move(tmp);
  }

  void operator()(geometry_port& out, geometry_port& in)
  {
    // OPTIMIZEME
    // if(out.flags & geometry_port::dirty_meshes)
    in.geometry = out.geometry; //std::move(out.meshes);
    // if(out.flags & geometry_port::dirty_transform)
    in.transform = out.transform;
    in.flags = out.flags;
    out.flags = {};
  }
};

struct copy_data
{
  /// Value ///
  void
  operator()(const value_vector<std::pair<ossia::typed_value, int>>& out, value_port& in)
  {
    // Called in local_pull_visitor
    for(auto& val : out)
      in.add_local_value(val.first);
  }
  void operator()(const value_vector<ossia::typed_value>& out, value_port& in)
  {
    // Called in copy_data_pos, when copying from a delay line to a port
    for(auto& val : out)
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
#if !defined(OSSIA_FREESTANDING)
    // Called in env_writer, when copying from a node to a delay line
    value_vector<ossia::typed_value> vec;
    vec.reserve(out.get_data().size());
    for(const ossia::timed_value& val : out.get_data())
    {
      vec.emplace_back(val, out.index, out.type);
    }
    in.data.push_back(std::move(vec));
#endif
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

  void operator()(const value_vector<libremidi::ump>& out, midi_port& in)
  {
    // Called in copy_data_pos, when copying from a delay line to a port
    for(const auto& data : out)
      in.messages.push_back(data);
  }

  void operator()(const midi_port& out, midi_delay_line& in)
  {
    // Called in env_writer, when copying from a node to a delay line
    in.messages.push_back(out.messages);
  }

  /// Geometry ///
  void operator()(const geometry_port& out, geometry_port& in)
  {
    // Called in init_node_visitor::copy, when copying from a node to another
    //if(out.flags & geometry_port::dirty_meshes)
    in.geometry = out.geometry;
    //if(out.flags & geometry_port::dirty_transform)
    in.transform = out.transform;
    in.flags = out.flags;
  }

  void operator()(const geometry_spec& out, geometry_port& in)
  {
    // Called in copy_data_pos below
    in.geometry = out;
  }

  void operator()(const geometry_port& out, geometry_delay_line& in)
  {
    // Called in env_writer, when copying from a node to a delay line
    // if(out.flags & geometry_port::dirty_meshes)
    in.geometries.push_back(out.geometry);
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
    if(pos < out.data.size())
    {
      copy_data{}(out.data[pos], in);
    }
  }

  void operator()(const audio_delay_line& out, audio_port& in)
  {
    if(pos < out.samples.size())
    {
      mix(out.samples[pos], in.get());
    }
  }

  void operator()(const midi_delay_line& out, midi_port& in)
  {
    if(pos < out.messages.size())
    {
      copy_data{}(out.messages[pos], in);
    }
  }
  void operator()(const geometry_delay_line& out, geometry_port& in)
  {
    if(pos < out.geometries.size())
    {
      copy_data{}(out.geometries[pos], in);
    }
  }
};
}
