#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <chobo/small_vector.hpp>
namespace ossia
{
template <typename T>
using value_vector = chobo::small_vector<T, 4>;

using audio_channel = chobo::small_vector<double, 64>;
using audio_vector = std::vector<audio_channel>;

struct audio_port
{
  bool upmix{};
  audio_vector samples;
};

struct midi_port
{
  value_vector<mm::MidiMessage> messages;
};

struct value_port
{
  ossia::complex_type type;
  value_vector<ossia::value> data;
};

struct audio_delay_line
{
  std::vector<audio_vector> samples;
};

struct midi_delay_line
{
  std::vector<value_vector<mm::MidiMessage>> messages;
};

struct value_delay_line
{
  ossia::complex_type type;
  std::vector<value_vector<ossia::value>> data;
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
  void operator()(const value_vector<ossia::value>& out, value_vector<ossia::value>& in)
  {
    for(auto& data : out)
      in.push_back(data);
  }

  void copy_audio(const chobo::small_vector<double, 64>& src, chobo::small_vector<double, 64>& sink)
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

  void operator()(const value_vector<mm::MidiMessage>& out, value_vector<mm::MidiMessage>& in)
  {
    for (const auto& data : out)
      in.push_back(data);
  }
};

struct copy_data
{
  template <typename T, typename U>
  void operator()(const T&, const U&) const
  {
  }

  void operator()(const value_port& out, value_delay_line& in)
  {
    in.data.push_back(out.data);
  }

  void operator()(const audio_port& out, audio_delay_line& in)
  {
    in.samples.push_back(out.samples);
  }

  void operator()(const midi_port& out, midi_delay_line& in)
  {
    in.messages.push_back(out.messages);
  }

  void operator()(const value_port& out, value_port& in)
  {
    mix{}(out.data, in.data);
  }

  void operator()(const audio_port& out, audio_port& in)
  {
    mix{}(out.samples, in.samples, in.upmix);
  }

  void operator()(const midi_port& out, midi_port& in)
  {
    mix{}(out.messages, in.messages);
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
      mix{}(out.data[pos], in.data);
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
      mix{}(out.messages[pos], in.messages);
    }
  }
};
}
