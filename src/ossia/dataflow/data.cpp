#include <ossia/dataflow/value_port.hpp>
#include <ossia/dataflow/data_copy.hpp>
#include <ossia/detail/algorithms.hpp>

namespace ossia
{
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

void value_port::add_port_values(const value_port& other)
{
  // These values come from another node: we just copy them blindly
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
