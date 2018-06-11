#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/domain/domain_base.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <rtmidi17/message.hpp>
namespace ossia
{
template <typename T>
using value_vector = ossia::small_vector<T, 4>;

using audio_channel = ossia::small_vector<double, 256>;
using audio_vector = ossia::small_vector<audio_channel, 2>;

struct audio_port
{
  bool upmix{};
  audio_vector samples;
};

struct midi_port
{
  value_vector<rtmidi::message> messages;
};

struct tvalue {
  explicit tvalue(ossia::value&& v): value{std::move(v)} { }
  tvalue(ossia::value&& v, const destination_index& d): value{std::move(v)}, index{d} { }
  tvalue(ossia::value&& v, const destination_index& d, const unit_t& u): value{std::move(v)}, index{d}, type{u} { }
  explicit tvalue(const ossia::value& v): value{v} { }
  tvalue(const ossia::value& v, const destination_index& d): value{v}, index{d} { }
  tvalue(const ossia::value& v, const destination_index& d, const unit_t& u): value{v}, index{d}, type{u} { }

  tvalue() = default;
  ~tvalue() = default;
  tvalue(const tvalue&) = default;
  tvalue(tvalue&&) = default;
  tvalue& operator=(const tvalue&) = default;
  tvalue& operator=(tvalue&&) = default;

  ossia::value value{};
  int64_t timestamp{};
  destination_index index{};
  ossia::complex_type type{};
};

enum data_mix_method: int8_t
{
  mix_append,
  mix_replace,
  mix_merge
};

struct value_port
{
  void add_raw_value(const ossia::tvalue& v)
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::tvalue& val) { return val.timestamp == v.timestamp; });
        if(it != data.end())
          *it = v;
        else
          data.emplace_back(v);
        break;
      }
      case data_mix_method::mix_append:
      {
        data.emplace_back(v);
        break;
      }
      case data_mix_method::mix_merge:
        // TODO;
        break;
    }
  }
  void add_raw_value(ossia::tvalue&& v)
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::tvalue& val) { return val.timestamp == v.timestamp; });
        if(it != data.end())
          *it = std::move(v);
        else
          data.emplace_back(std::move(v));
        break;
      }
      case data_mix_method::mix_append:
      {
        data.emplace_back(std::move(v));
        break;
      }
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }

  void add_raw_value(ossia::value&& v)
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::tvalue& val) { return val.timestamp == 0; });
        if(it != data.end())
          *it = ossia::tvalue{std::move(v)};
        else
          data.emplace_back(std::move(v));
        break;
      }
      case data_mix_method::mix_append:
        data.emplace_back(std::move(v));
        break;
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }

  /*
  void add_external_value(const ossia::net::parameter_base& addr)
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::tvalue& val) { return val.timestamp == 0; });
        if(it != data.end())
          *it = addr.value();
        else
          data.emplace_back(ossia::convert(addr.value(), type));
        break;
      }
      case data_mix_method::mix_append:
        data.emplace_back(addr.value());
        break;
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }

  void add_external_value(const ossia::net::parameter_base& addr, ossia::value&& v)
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::tvalue& val) { return val.timestamp == 0; });
        if(it != data.end())
          *it = addr.value();
        else
          data.emplace_back(ossia::convert(v, type));
        break;
      }
      case data_mix_method::mix_append:
        data.emplace_back(addr.value());
        break;
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }
  */

  void add_value(ossia::value&& v, int64_t timestamp)
  {
    auto setup_value = [=] (ossia::tvalue& tval) {
      tval.timestamp = timestamp;
      tval.index = this->index;
      if(auto u = this->type.target<ossia::unit_t>())
        tval.type = *u;
    };
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::tvalue& val) { return val.timestamp == timestamp; });
        if(it != data.end())
        {
          it->value = std::move(v);
          setup_value(*it);
        }
        else
        {
          data.emplace_back(std::move(v));
          setup_value(this->data.back());
        }
        break;
      }
      case data_mix_method::mix_append:
      {
        this->data.emplace_back(std::move(v));
        setup_value(this->data.back());
        break;
      }
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }


  void set_data(const value_vector<ossia::tvalue>& vec)
  {
    data = vec;
  }

  void clear()
  {
    data.clear();
  }

  const value_vector<ossia::tvalue>& get_data() const { return data; }
  value_vector<ossia::tvalue>& get_data() { return data; }

  ossia::domain domain;
  ossia::bounding_mode bounding{};
  ossia::complex_type type;
  ossia::destination_index index;

  bool is_event{};
  data_mix_method mix_method{};
private:
  value_vector<ossia::tvalue> data;
};

struct audio_delay_line
{
  std::vector<audio_vector> samples;
};

struct midi_delay_line
{
  std::vector<value_vector<rtmidi::message>> messages;
};

struct value_delay_line
{
  std::vector<value_vector<ossia::tvalue>> data;

  ossia::domain domain;
  ossia::bounding_mode bounding{};
  ossia::repetition_filter rep_filter{};
  ossia::complex_type type;
};

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
  void operator()(const value_vector<std::pair<ossia::tvalue, int>>& out, value_port& in)
  {
    for(auto& val : out)
      in.add_raw_value(val.first);
  }
  void operator()(const value_vector<ossia::tvalue>& out, value_port& in)
  {
    for(auto& val : out)
      in.add_raw_value(val);
  }

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

  void operator()(const value_vector<rtmidi::message>& out, value_vector<rtmidi::message>& in)
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

  void operator()(const value_vector<ossia::tvalue>& out, value_delay_line& in)
  {
    in.data.push_back(out);
  }
  void operator()(const value_vector<std::pair<ossia::tvalue, int>>& out, value_delay_line& in)
  {
    value_vector<ossia::tvalue> v;
    v.reserve(out.size());
    for(auto& val : out)
      v.push_back(val.first);
    in.data.push_back(std::move(v));
  }

  void operator()(const value_port& out, value_delay_line& in)
  {
    in.data.push_back(out.get_data());
  }

  void operator()(const audio_port& out, audio_delay_line& in)
  {
    in.samples.push_back(out.samples);
  }

  void operator()(const midi_port& out, midi_delay_line& in)
  {
    in.messages.push_back(out.messages);
  }

  void operator()(const value_vector<std::pair<ossia::tvalue, int>>& out, value_port& in)
  {
    mix{}(out, in);
  }
  void operator()(const value_vector<ossia::tvalue>& out, value_port& in)
  {
    mix{}(out, in);
  }

  void operator()(const value_port& out, value_port& in)
  {
    mix{}(out.get_data(), in);
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
      mix{}(out.data[pos], in);
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
