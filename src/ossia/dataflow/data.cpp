#include <ossia/dataflow/data_copy.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/common/value_bounding.hpp>
#include <ossia/network/common/value_mapping.hpp>
#include <ossia/network/value/format_value.hpp>

namespace ossia
{
namespace
{

static inline ossia::complex_type
get_complex_type(const ossia::net::parameter_base& other) noexcept
{
  if(const auto u = other.get_unit())
    return u;
  return other.get_value_type();
}

// TODO we should take a visitor that takes (value, source_domain, sink_domain)
// for each argument and does the right thing
struct process_float_control_visitor
{
  float src_min{};
  float dst_min{};
  float ratio{};

  void operator()(ossia::impulse) const noexcept { }

  void operator()(int32_t& v) const noexcept { v = dst_min + ratio * (v - src_min); }
  void operator()(float& v) const noexcept { v = dst_min + ratio * (v - src_min); }
  void operator()(bool& v) const noexcept { v = dst_min + ratio * (v - src_min); }

  void operator()(char& v) const noexcept { v = dst_min + ratio * (v - src_min); }
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
  void operator()(value_map_type& v) const noexcept
  {
    for(auto& [k, value] : v)
      value.apply(*this);
  }

  void operator()() const noexcept { }
};

bool should_process_control(
    const value_port& source_port, const value_port& sink_port) noexcept
{
  return (source_port.domain && sink_port.domain) || (source_port.type && sink_port.type)
      // FIXME   || source_port.tween_date
      ;
}

void process_float_control(
    ossia::value& v, float src_min, float src_max, float dst_min, float dst_max)
{
  const float sub = src_max - src_min;
  if(sub == 0.f)
    return;

  const float ratio = (dst_max - dst_min) / sub;
  v.apply(process_float_control_visitor{src_min, dst_min, ratio});
}

void process_control_value(
    ossia::value& v, const value_port& source_port, const value_port& sink_port) noexcept
{
  if(source_port.domain && sink_port.domain && source_port.type && sink_port.type)
    process_control_value(
        v, source_port.domain, sink_port.domain, source_port.type, sink_port.type);
  else if(source_port.type && sink_port.type)
    process_control_value(v, source_port.type, sink_port.type);
  else if(source_port.domain && sink_port.domain)
    process_control_value(v, source_port.domain, sink_port.domain);
  // FIXME  if(source_port.tween_date)
  // FIXME  {
  // FIXME    // TODO
  // FIXME  }
}
}

void process_control_value(
    ossia::value& v, const ossia::domain& source_domain,
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
    ossia::value& v, const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type) noexcept
{
  v = convert(v, source_type, sink_type);
}

void process_control_value(
    ossia::value& v, const ossia::domain& source_domain,
    const ossia::domain& sink_domain, const ossia::complex_type& source_type,
    const ossia::complex_type& sink_type) noexcept
{
  // FIXME check that it works:
  // with an int source (e.g. a MIDI address)
  // with an int sink (e.g. an IntSlider port)
  // with both int source and sink
  process_control_value(v, source_domain, sink_domain); // TODO does that make sense
  process_control_value(v, source_type, sink_type);
}

void ensure_vector_sizes(const audio_vector& src_vec, audio_vector& sink_vec)
{
  const auto src_chans = src_vec.size();
  const auto sink_chans = sink_vec.size();
  if(sink_chans < src_chans)
    audio_buffer_pool::set_channels(sink_vec, src_chans);

  for(std::size_t chan = 0; chan < src_chans; chan++)
  {
    const std::size_t N = src_vec[chan].size();
    if(sink_vec[chan].size() < N)
      sink_vec[chan].resize(N);
  }
}

void mix(const audio_vector& src_vec, audio_vector& sink_vec)
{
  if(BOOST_UNLIKELY(src_vec.size() != 0 && sink_vec.size() == 0))
  {
    const auto channels = src_vec.size();
    audio_buffer_pool::set_channels(sink_vec, channels);
    for(std::size_t c = 0; c < channels; c++)
    {
      const auto& src = src_vec[c];
      sink_vec[c].assign(src.begin(), src.end());
    }
    return;
  }
  else if(BOOST_LIKELY(src_vec.size() == sink_vec.size()))
  {
    for(std::size_t chan = 0, src_chans = src_vec.size(); chan < src_chans; chan++)
    {
      auto& src = src_vec[chan];
      auto& sink = sink_vec[chan];
      if(sink.empty())
      {
        sink = src;
      }
      else
      {
        const std::size_t N = src.size();

        if(BOOST_UNLIKELY(sink.size() < N))
        {
          sink.resize(N);
        }

        auto src_p = src.data();
        auto sink_p = sink.data();
        for(std::size_t i = 0; i < N; i++)
          sink_p[i] += src_p[i];
      }
    }
  }
  else
  {
    ensure_vector_sizes(src_vec, sink_vec);
    // Just copy the channels without much thoughts
    for(std::size_t chan = 0, src_chans = src_vec.size(); chan < src_chans; chan++)
    {
      auto& src = src_vec[chan];
      auto& sink = sink_vec[chan];
      const std::size_t N = src.size();
      auto src_p = src.data();
      auto sink_p = sink.data();

      for(std::size_t i = 0; i < N; i++)
        sink_p[i] += src_p[i];
    }
  }
}

void audio_buffer_pool::set_channels(audio_vector& samples, std::size_t channels)
{
  if(samples.size() == channels)
    return;

  auto& pool = audio_buffer_pool::instance();
  while(samples.size() > channels)
  {
    auto chan = std::move(samples.back());
    chan.clear();

    pool.release(std::move(chan));
    samples.pop_back();
  }

  samples.reserve(channels);
  while(samples.size() < channels)
  {
    samples.push_back(pool.acquire());
  }
}

void value_port::write_value(const value& v, int64_t timestamp)
{
  switch(mix_method)
  {
    case data_mix_method::mix_replace: {
      auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
        return val.timestamp == timestamp;
      });
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
    case data_mix_method::mix_append: {
      this->data.emplace_back(v);
      break;
    }
    case data_mix_method::mix_merge: {
      // TODO;
      break;
    }
  }
}

void value_port::write_value(value&& v, int64_t timestamp)
{
  switch(mix_method)
  {
    case data_mix_method::mix_replace: {
      auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
        return val.timestamp == timestamp;
      });
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
    case data_mix_method::mix_append: {
      this->data.emplace_back(std::move(v));
      break;
    }
    case data_mix_method::mix_merge: {
      // TODO;
      break;
    }
  }
}

static void filter_value(
    value& source, const ossia::destination_index& /* source_index */,
    const ossia::destination_index& res_index, const ossia::complex_type& source_type,
    const ossia::complex_type& res_type)
{
  // 1. Convert from source unit to destination unit
  auto src_unit = source_type.target<ossia::unit_t>();
  auto tgt_unit = source_type.target<ossia::unit_t>();
  if(src_unit && tgt_unit && *src_unit != *tgt_unit)
  {
    source = ossia::convert(source, *src_unit, *tgt_unit);
  }
  else
  {
    auto src_type = source_type.target<ossia::val_type>();
    auto tgt_type = res_type.target<ossia::val_type>();
    if(src_type && tgt_type && *src_type != *tgt_type && res_index.empty())
    {
      ossia::convert_inplace(source, *tgt_type);
    }
  }

  if(source.valid() && !res_index.empty())
  {
    source = get_value_at_index(source, res_index);
  }
}

void value_port::add_local_value(const ossia::typed_value& other)
{
  // These values come from the local environment
  // Convert to the correct type / index
  if(other.index == index && other.type == type)
  {
    write_value(other.value, other.timestamp);
  }
  else
  {
    auto v = other.value;
    filter_value(v, other.index, index, other.type, type);
    write_value(v, other.timestamp);
  }
}

void value_port::add_global_values(
    const net::parameter_base& other, const value_vector<ossia::value>& vec)
{
  const ossia::complex_type source_type = get_complex_type(other);

  if(index.empty() && (source_type == type || !source_type))
  {
    const auto& other_domain = other.get_domain();
    if(other_domain && this->domain)
    {
      for(ossia::value v : vec)
      {
        map_value(v, index, other_domain, this->domain);
        write_value(std::move(v), 0); // TODO put correct timestamps here
      }
    }
    else
    {
      for(const ossia::value& v : vec)
      {
        write_value(v, 0);
      }
    }
  }
  else
  {
    const auto& other_domain = other.get_domain();
    if(other_domain && this->domain)
    {
      for(ossia::value v : vec)
      {
        filter_value(v, {}, index, source_type, type);
        map_value(v, index, other_domain, this->domain);
        write_value(std::move(v), 0);
      }
    }
    else
    {
      for(ossia::value v : vec)
      {
        filter_value(v, {}, index, source_type, type);
        write_value(v, 0);
      }
    }
  }
}

void value_port::add_global_value(
    const ossia::net::parameter_base& other, ossia::value val)
{
  const ossia::complex_type source_type = get_complex_type(other);

  // Convert to the correct unit
  filter_value(val, {}, index, source_type, type);

  // Map the value
  if(other.get_domain() && this->domain)
  {
    map_value(val, index, other.get_domain(), this->domain);
  }

  // Write the value
  write_value(std::move(val), 0);
}

void value_port::add_port_values(const value_port& other)
{
  // These values come from another node: we just copy them blindly
  if(should_process_control(other, *this))
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace: {
        for(const auto& v : other.get_data())
        {
          auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
            return val.timestamp == v.timestamp;
          });
          if(it != data.end())
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
      case data_mix_method::mix_append: {
        auto it = data.insert(data.end(), other.data.begin(), other.data.end());
        for(const auto end = data.end(); it != end; ++it)
        {
          process_control_value(it->value, other, *this);
        }
        break;
      }
      case data_mix_method::mix_merge: {
        // TODO;
        break;
      }
    }
  }
  else
  {
    switch(mix_method)
    {
      case data_mix_method::mix_replace: {
        for(const auto& v : other.get_data())
        {
          auto it = ossia::find_if(data, [&](const ossia::timed_value& val) {
            return val.timestamp == v.timestamp;
          });
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
      case data_mix_method::mix_append: {
        data.insert(data.end(), other.data.begin(), other.data.end());
        break;
      }
      case data_mix_method::mix_merge: {
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

void geometry_port::clear()
{
  flags = {};
}

}
