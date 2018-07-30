#pragma once
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/oscquery/detail/value_to_json.hpp>

namespace ossia
{
namespace oscquery
{
namespace detail
{
inline void
write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, int v)
{
  writer.Int(v);
}
inline void
write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, double v)
{
  writer.Double(v);
}
inline void
write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, float v)
{
  writer.Double(v);
}
inline void
write_json(rapidjson::Writer<rapidjson::StringBuffer>& writer, bool v)
{
  writer.Bool(v);
}
inline void write_json(
    rapidjson::Writer<rapidjson::StringBuffer>& writer, const std::string& v)
{
  writer.String(v);
}
inline void write_json(
    rapidjson::Writer<rapidjson::StringBuffer>& writer, const ossia::value& v)
{
  v.apply(value_to_json{writer});
}

//! Write a domain to json.
struct domain_to_json
{
  rapidjson::Writer<rapidjson::StringBuffer>& writer;
  void operator()()
  {
    writer.Null();
  }

  void operator()(const ossia::domain_base<impulse>& dom)
  {
    writer.Null();
  }

  void operator()(const ossia::domain_base<bool>& dom)
  {
    writer.StartArray();
    writer.StartObject();
    writer.Key("VALS");
    writer.StartArray();
    write_json(writer, false);
    write_json(writer, true);
    writer.EndArray();
    writer.EndObject();
    writer.EndArray();
  }

  template <typename T>
  void operator()(const ossia::domain_base<T>& dom)
  {
    bool has_min = bool(dom.min);
    bool has_max = bool(dom.max);
    bool has_values = !dom.values.empty();
    if (has_min || has_max || has_values)
    {
      writer.StartArray();
      writer.StartObject();
      if (has_min)
      {
        writer.Key("MIN");
        write_json(writer, *dom.min);
      }
      if (has_max)
      {
        writer.Key("MAX");
        write_json(writer, *dom.max);
      }
      if (has_values)
      {
        writer.Key("VALS");
        writer.StartArray();
        for (auto val : dom.values)
          write_json(writer, val);
        writer.EndArray();
      }
      writer.EndObject();
      writer.EndArray();
    }
    else
    {
      writer.Null();
    }
  }

  void operator()(const ossia::domain_base<std::string>& dom)
  {
    if (!dom.values.empty())
    {
      writer.StartArray();
      writer.StartObject();
      writer.Key("VALS");
      writer.StartArray();
      for (auto val : dom.values)
        write_json(writer, val);
      writer.EndArray();
      writer.EndObject();
      writer.EndArray();
    }
    else
    {
      writer.Null();
    }
  }

  void operator()(const ossia::vector_domain& dom)
  {
    const auto min_count = dom.min.size();
    const auto max_count = dom.max.size();
    const auto values_count = dom.values.size();
    const auto N = std::max(std::max(min_count, max_count), values_count);
    if (N > 0)
    {
      writer.StartArray();
      for (std::size_t i = 0; i < N; i++)
      {
        if (values_count > i && !dom.values[i].empty())
        {
          writer.StartObject();

          writer.Key("VALS");
          writer.StartArray();

          for (const auto& val : dom.values[i])
            write_json(writer, val);

          writer.EndArray();
          writer.EndObject();
        }
        else if (
                 (min_count > i && dom.min[i].valid())
                 || (max_count > i && dom.max[i].valid()))
        {
          writer.StartObject();
          if (dom.min[i].valid())
          {
            writer.Key("MIN");
            write_json(writer, dom.min[i]);
          }

          if (dom.max[i].valid())
          {
            writer.Key("MAX");
            write_json(writer, dom.max[i]);
          }
          writer.EndObject();
        }
        else
        {
          writer.Null();
        }
      }
      writer.EndArray();
    }
    else
    {
      writer.Null();
    }
  }

  template <std::size_t N>
  void operator()(const ossia::vecf_domain<N>& dom)
  {
    writer.StartArray();
    for (std::size_t i = 0; i < N; i++)
    {
      if (!dom.values[i].empty())
      {
        writer.StartObject();

        writer.Key("VALS");
        writer.StartArray();

        for (const auto val : dom.values[i])
          writer.Double(val);

        writer.EndArray();
        writer.EndObject();
      }
      else if (dom.min[i] || dom.max[i])
      {
        writer.StartObject();
        if (dom.min[i])
        {
          writer.Key("MIN");
          write_json(writer, *dom.min[i]);
        }

        if (dom.max[i])
        {
          writer.Key("MAX");
          write_json(writer, *dom.max[i]);
        }
        writer.EndObject();
      }
      else
      {
        writer.Null();
      }
    }
    writer.EndArray();
  }

  void operator()(const ossia::domain_base<ossia::value>& dom)
  {
    /* TODO this code is not correct
    if (dom.min)
    {
      dom.min->apply(value_to_json{writer});
    }
    else
    {
      writer.Null();
    }

    if (dom.max)
    {
      dom.max->apply(value_to_json{writer});
    }
    else
    {
      writer.Null();
    }

    if (!dom.values.empty())
    {
      writer.StartArray();
      for (const auto& val : dom.values)
      {
        val.apply(value_to_json{writer});
      }
      writer.EndArray();
    }
    else
    {
      writer.Null(); // TODO why not just nothing ?
    }
    */
  }
};

}
}
}
