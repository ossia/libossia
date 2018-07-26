#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/domain/domain_base.hpp>
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

struct timed_value {
  explicit timed_value(ossia::value&& v): value{std::move(v)} { }
  explicit timed_value(const ossia::value& v): value{v} { }
  explicit timed_value(ossia::value&& v, int64_t ts): value{std::move(v)}, timestamp{ts} { }
  explicit timed_value(const ossia::value& v, int64_t ts): value{v}, timestamp{ts} { }

  timed_value() = default;
  ~timed_value() = default;
  timed_value(const timed_value&) = default;
  timed_value(timed_value&&) = default;
  timed_value& operator=(const timed_value&) = default;
  timed_value& operator=(timed_value&&) = default;

  ossia::value value{};
  int64_t timestamp{};
};

struct typed_value {
  explicit typed_value(ossia::value&& v)
    : value{std::move(v)} { }
  explicit typed_value(ossia::timed_value&& v)
    : value{std::move(v.value)}, timestamp{v.timestamp} { }
  typed_value(ossia::value&& v, const destination_index& d)
    : value{std::move(v)}, index{d} { }
  typed_value(ossia::value&& v, const destination_index& d, const ossia::complex_type& u)
    : value{std::move(v)}, index{d}, type{u} { }
  typed_value(ossia::timed_value&& v, const destination_index& d, const ossia::complex_type& u)
    : value{std::move(v.value)}, timestamp{v.timestamp}, index{d}, type{u} { }

  explicit typed_value(const ossia::value& v)
    : value{v} { }
  explicit typed_value(const ossia::timed_value& v)
    : value{v.value}, timestamp{v.timestamp} { }
  typed_value(const ossia::value& v, const destination_index& d)
    : value{v}, index{d} { }
  typed_value(const ossia::value& v, const destination_index& d, const ossia::complex_type& u)
    : value{v}, index{d}, type{u} { }
  typed_value(const ossia::timed_value& v, const destination_index& d, const ossia::complex_type& u)
    : value{v.value}, timestamp{v.timestamp}, index{d}, type{u} { }

  typed_value() = default;
  ~typed_value() = default;
  typed_value(const typed_value&) = default;
  typed_value(typed_value&&) = default;
  typed_value& operator=(const typed_value&) = default;
  typed_value& operator=(typed_value&&) = default;

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
  //! Use this function to write from a node to an output port
  void write_value(const ossia::value& v, int64_t timestamp)
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::timed_value& val) { return val.timestamp == timestamp; });
        if(it != data.end())
        {
          it->value = v;
        }
        else
        {
          data.emplace_back(v);
        }
        break;
      }
      case data_mix_method::mix_append:
      {
        this->data.emplace_back(v);
        break;
      }
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }

  void write_value(ossia::value&& v, int64_t timestamp)
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        auto it = ossia::find_if(data, [&] (const ossia::timed_value& val) { return val.timestamp == timestamp; });
        if(it != data.end())
        {
          it->value = std::move(v);
        }
        else
        {
          data.emplace_back(std::move(v));
        }
        break;
      }
      case data_mix_method::mix_append:
      {
        this->data.emplace_back(std::move(v));
        break;
      }
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }

  ossia::value filter_value(
      const ossia::value& source,
      const ossia::destination_index& source_idx, // TODO handle me
      const ossia::complex_type& source_type) const
  {
    if(source_type == type || !source_type)
    {
      // We can just take the value at the index
      return get_value_at_index(source, this->index);
    }
    else
    {
      auto res = ossia::convert(source, source_type, type);
      if(res.valid())
      {
        return get_value_at_index(res, this->index);
      }
    }
    return source;
  }

  void add_local_value(const ossia::typed_value& other)
  {
    // These values come from the local environemnt
    // Convert to the correct type / index
    if(other.index == index && other.type == type)
    {
      write_value(other.value, other.timestamp);
    }
    else
    {
      write_value(filter_value(other.value, other.index, other.type), other.timestamp);
    }
  }

  void add_port_values(const ossia::value_port& other)
  {
    // These values come from another node: we just copy them blindly
    switch(mix_method)
    {
      case data_mix_method::mix_replace:
      {
        for(const auto& v : other.get_data())
        {
          auto it = ossia::find_if(data, [&] (const ossia::timed_value& val) { return val.timestamp == v.timestamp; });
          if(it != data.end())
          {
            it->value = v.value;
          }
          else
          {
            data.emplace_back(v);
          }
        }
        break;
      }
      case data_mix_method::mix_append:
      {
        data.insert(data.end(), other.data.begin(), other.data.end());
        break;
      }
      case data_mix_method::mix_merge:
      {
        // TODO;
        break;
      }
    }
  }


  void add_global_values(const ossia::net::parameter_base& param, const value_vector<ossia::value>& vec);

  void add_global_value(const ossia::net::parameter_base& other, const ossia::value& v);

  void set_data(const value_vector<ossia::timed_value>& vec)
  {
    data = vec;
  }

  void clear()
  {
    data.clear();
  }

  const value_vector<ossia::timed_value>& get_data() const { return data; }
  value_vector<ossia::timed_value>& get_data() { return data; }

  ossia::domain domain;
  ossia::bounding_mode bounding{};
  ossia::complex_type type;
  ossia::destination_index index;

  bool is_event{};
  data_mix_method mix_method{};
  ossia::timed_value t;
private:
  value_vector<ossia::timed_value> data;
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
  std::vector<value_vector<ossia::typed_value>> data;
};

}
