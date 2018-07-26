#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/parameter.hpp>
namespace ossia
{

inline void value_port::add_global_values(
    const net::parameter_base& other
    , const value_vector<ossia::value>& vec)
{
  const ossia::complex_type source_type = other.get_unit();
  const ossia::destination_index source_idx{};

  if(source_idx == index && source_type == type)
  {
    for(const ossia::value& v : vec)
      write_value(v, 0);
  }
  else
  {
    for(const ossia::value& v : vec)
      write_value(filter_value(v, source_idx, source_type), 0);
  }
}

inline void value_port::add_global_value(
    const ossia::net::parameter_base& other
    , const ossia::value& v)
{
  const ossia::complex_type source_type = other.get_unit();
  const ossia::destination_index source_idx{};
  if(source_idx == index && source_type == type)
  {
    write_value(v, 0);
  }
  else
  {
    write_value(filter_value(v, source_idx, source_type), 0);
  }
}

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
    for(auto& vec : p.samples)
      vec.clear();
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


struct mix
{
  void copy_audio(const ossia::audio_channel& src, ossia::audio_channel& sink)
  {
    if(sink.size() < src.size())
      sink.resize(src.size());
    std::size_t N = src.size();
    for(std::size_t i = 0; i < N; i++)
      sink[i] += src[i];
  }
  void operator()(const audio_vector& src_vec, audio_vector& sink_vec, bool upmix)
  {
    const auto src_chans = src_vec.size();
    const auto sink_chans = sink_vec.size();
    if(upmix && src_chans != sink_chans && sink_chans != 0)
    {
      // Try to mix input data "meaningfully" without loosing information
      if(src_vec.size() > sink_vec.size())
      {
        // Upmix sink_vec in src_vec
        // OPTIMIZEME by resizing and copying in one go instead of the two steps here
        switch(sink_chans)
        {
          case 1:
          {
            sink_vec.resize(src_chans);
            for(std::size_t i = 1; i < src_chans; i++)
              sink_vec[i] = sink_vec[0];
            break;
          }
          case 2:
          {
            // LRL2R2
            sink_vec.resize(src_chans);
            for(std::size_t i = 1; i < src_chans; i+=2)
            {
              sink_vec[i] = sink_vec[0];
              sink_vec[i+1] = sink_vec[1];
            }
            break;
          }
          default:
            // Nothing really meaningful can be done
            break;
        }

        for(std::size_t chan = 0; chan < src_chans; chan++)
        {
          auto& src = src_vec[chan];
          auto& sink = sink_vec[chan];
          copy_audio(src, sink);
        }
      }
      else
      {
        // Upmix src_vec in sink_vec
        switch(src_chans)
        {
          case 1:
          {
            auto& src = src_vec[0];
            for(std::size_t chan = 0; chan < sink_chans; chan++)
            {
              auto& sink = sink_vec[chan];
              copy_audio(src, sink);
            }
            break;
          }
          case 2:
          {
            // LRL2R2
            auto& src_l = src_vec[0];
            auto& src_r = src_vec[1];
            for(std::size_t chan = 0; chan < sink_chans; chan+=2)
            {
              copy_audio(src_l, sink_vec[chan]);
              copy_audio(src_r, sink_vec[chan+1]);
            }
            break;
          }
          default:
            // Nothing really meaningful can be done
            break;
        }
      }
    }
    else
    {
      // Just copy the channels without much thoughts
      if(sink_chans < src_chans)
        sink_vec.resize(src_chans);

      for(std::size_t chan = 0; chan < src_chans; chan++)
      {
        auto& src = src_vec[chan];
        auto& sink = sink_vec[chan];
        copy_audio(src, sink);
      }
    }
  }
};

struct copy_data
{
  /// Value ///
  void operator()(const value_vector<std::pair<ossia::typed_value, int>>& out, value_port& in)
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

  void operator()(const net::parameter_base& param, const value_vector<ossia::value>& vec, value_port& in)
  {
    // Called from global_pull_visitor
    in.add_global_values(param, vec);
  }

  void operator()(const value_port& out, value_delay_line& in)
  {
    // Called in env_writer, when copying from a node to a delay line
    value_vector<ossia::typed_value> vec;
    vec.reserve(out.get_data().size());
    for(const ossia::timed_value& val : out.get_data())
    {
      vec.emplace_back(val, out.index, out.type);
    }
    in.data.push_back(std::move(vec));
  }


  /// Audio ///
  void operator()(const audio_port& out, audio_delay_line& in)
  {
    // Called in env_writer, when copying from a node to a delay line
    in.samples.push_back(out.samples);
  }

  void operator()(const audio_port& out, audio_port& in)
  {
    // Called in init_node_visitor::copy, when copying from a node to another
    mix{}(out.samples, in.samples, in.upmix);
  }


  /// MIDI ///
  void operator()(const midi_port& out, midi_port& in)
  {
    // Called in init_node_visitor::copy, when copying from a node to another
    operator()(out.messages, in);
  }

  void operator()(const value_vector<rtmidi::message>& out, midi_port& in)
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
      mix{}(out.samples[pos], in.samples, in.upmix);
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
