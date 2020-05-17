#include <ossia/dataflow/value_port.hpp>
#include <ossia/dataflow/data_copy.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/network/common/complex_type.hpp>

namespace ossia
{
namespace
{
// TODO we should take a visitor that takes (value, source_domain, sink_domain)
// for each argument and does the right thing
struct process_float_control_visitor
{
  float src_min{};
  float dst_min{};
  float ratio{};

  void operator()(ossia::impulse) const noexcept
  {

  }

  void operator()(int& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }
  void operator()(float& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }
  void operator()(bool& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }

  void operator()(char& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }
  void operator()(std::string& str) const noexcept
  {
    for(char& v : str)
      v = dst_min + ratio * (v - src_min);
  }

  void operator()(ossia::vec2f& v) const noexcept
  {
    v[0] = dst_min + ratio * (v[0] - src_min);
    v[1] = dst_min + ratio * (v[1] - src_min);
  }
  void operator()(ossia::vec3f& v) const noexcept
  {
    v[0] = dst_min + ratio * (v[0] - src_min);
    v[1] = dst_min + ratio * (v[1] - src_min);
    v[2] = dst_min + ratio * (v[2] - src_min);
  }
  void operator()(ossia::vec4f& v) const noexcept
  {
    v[0] = dst_min + ratio * (v[0] - src_min);
    v[1] = dst_min + ratio * (v[1] - src_min);
    v[2] = dst_min + ratio * (v[2] - src_min);
    v[3] = dst_min + ratio * (v[3] - src_min);
  }

  void operator()(std::vector<ossia::value>& v) const noexcept
  {
    for(auto& value : v)
      value.apply(*this);
  }

  void operator()() const noexcept
  {
  }
};

bool should_process_control(
    const value_port& source_port,
    const value_port& sink_port) noexcept
{
  return (source_port.domain && sink_port.domain) || (source_port.type && sink_port.type) || source_port.tween_date;
}

void process_float_control(
    ossia::value& v, float src_min, float src_max, float dst_min, float dst_max)
{
  const float sub = src_max - src_min ;
  if(sub == 0.f)
    return;

  const float ratio = (dst_max - dst_min) / sub;
  v.apply(process_float_control_visitor{src_min, dst_min, ratio});
}

void process_control_value(
    ossia::value& v,
    const value_port& source_port,
    const value_port& sink_port) noexcept
{
  if(source_port.domain && sink_port.domain && source_port.type && sink_port.type)
    process_control_value(v, source_port.domain, sink_port.domain, source_port.type, sink_port.type);
  else if(source_port.type && sink_port.type)
    process_control_value(v, source_port.type, sink_port.type);
  else if(source_port.domain && sink_port.domain )
    process_control_value(v, source_port.domain, sink_port.domain);
  if (source_port.tween_date)
  {
    // TODO
  }
}
}

void process_control_value(
    ossia::value& v,
    const ossia::domain& source_domain,
    const ossia::domain& sink_domain) noexcept
{
  auto [src_min, src_max] = ossia::get_float_minmax(source_domain);
  auto [tgt_min, tgt_max] = ossia::get_float_minmax(sink_domain);

  if(src_min && src_max && tgt_min && tgt_max)
  {
    process_float_control(v, *src_min, *src_max, *tgt_min, *tgt_max);
  }
}


void process_control_value(
    ossia::value& v,
    const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type) noexcept
{
  v = convert(v, source_type, sink_type);
}

void process_control_value(
    ossia::value& v,
    const ossia::domain& source_domain,
    const ossia::domain& sink_domain,
    const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type
    ) noexcept
{
  process_control_value(v, source_type, sink_type);
  process_control_value(v, source_domain, sink_domain); // TODO does that make sense
}

void ensure_vector_sizes(const audio_vector& src_vec, audio_vector& sink_vec)
{
  const auto src_chans = src_vec.size();
  const auto sink_chans = sink_vec.size();
  if (sink_chans < src_chans)
    sink_vec.resize(src_chans);
  for (std::size_t chan = 0; chan < src_chans; chan++)
  {
    const std::size_t N = src_vec[chan].size();
    if (sink_vec[chan].size() < N)
      sink_vec[chan].resize(N);
  }
}


void value_port::write_value(const value& v, int64_t timestamp)
{
  switch (mix_method)
  {
  case data_mix_method::mix_replace:
  {
    auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
      return val.timestamp == timestamp;
    });
    if (it != data.end())
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

void value_port::write_value(value&& v, int64_t timestamp)
{
  switch (mix_method)
  {
  case data_mix_method::mix_replace:
  {
    auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
      return val.timestamp == timestamp;
    });
    if (it != data.end())
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

value value_port::filter_value(const value& source, const destination_index& source_idx, const ossia::complex_type& source_type) const
{
  if (source_type == type || !source_type)
  {
    // We can just take the value at the index
    return get_value_at_index(source, this->index);
  }
  else
  {
    auto res = ossia::convert(source, source_type, type);
    if (res.valid())
    {
      return get_value_at_index(res, this->index);
    }
  }
  return source;
}

void value_port::add_local_value(const ossia::typed_value& other)
{
  // These values come from the local environemnt
  // Convert to the correct type / index
  if (other.index == index && other.type == type)
  {
    write_value(other.value, other.timestamp);
  }
  else
  {
    write_value(
          filter_value(other.value, other.index, other.type), other.timestamp);
  }
}

void value_port::add_global_values(
    const net::parameter_base& other, const value_vector<ossia::value>& vec)
{
  const ossia::complex_type source_type = other.get_unit();
  const ossia::destination_index source_idx{}; // WTF?

  if (source_idx == index && source_type == type)
  {
    for (const ossia::value& v : vec)
      write_value(v, 0);
  }
  else
  {
    for (const ossia::value& v : vec)
      write_value(filter_value(v, source_idx, source_type), 0);
  }
}

void value_port::add_global_value(
    const ossia::net::parameter_base& other, const ossia::value& v)
{
  const ossia::complex_type source_type = other.get_unit();
  const ossia::destination_index source_idx{}; // WTF?
  if (source_idx == index && source_type == type)
  {
    write_value(v, 0);
  }
  else
  {
    write_value(filter_value(v, source_idx, source_type), 0);
  }
}


void value_port::add_port_values(const value_port& other)
{
  // These values come from another node: we just copy them blindly
  if (should_process_control(other, *this))
  {
    switch (mix_method)
    {
    case data_mix_method::mix_replace:
    {
      for (const auto& v : other.get_data())
      {
        auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
          return val.timestamp == v.timestamp;
        });
        if (it != data.end())
        {
          it->value = v.value;
          process_control_value(it->value, other, *this);
        }
        else
        {
          data.emplace_back(v);
          process_control_value(data.back().value, other, *this);
        }
      }
      break;
    }
    case data_mix_method::mix_append:
    {
      auto it = data.insert(data.end(), other.data.begin(), other.data.end());
      for(const auto end = data.end(); it != end; ++it) {
        process_control_value(it->value, other, *this);
      }
      break;
    }
    case data_mix_method::mix_merge:
    {
      // TODO;
      break;
    }
    }
  }
  else
  {
    switch (mix_method)
    {
    case data_mix_method::mix_replace:
    {
      for (const auto& v : other.get_data())
      {
        auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
          return val.timestamp == v.timestamp;
        });
        if (it != data.end())
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
}

void value_port::set_data(const value_vector<ossia::timed_value>& vec)
{
  data = vec;
}

void value_port::clear()
{
  data.clear();
}

const value_vector<ossia::timed_value>& value_port::get_data() const
{
  return data;
}

value_vector<ossia::timed_value>& value_port::get_data()
{
  return data;
}

}
