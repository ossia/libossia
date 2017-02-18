#pragma once
#include <ossia/network/oscquery/detail/value_to_json.hpp>
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
namespace oscquery
{
namespace detail
{

//! Write a domain to json.
struct domain_to_json
{
    rapidjson::Writer<rapidjson::StringBuffer>& writer;
    void operator()()
    {
        writer.Null();
        writer.Null();
        writer.Null();
    }
    void operator()(const ossia::domain_base<impulse> & dom)
    {
        writer.Null();
        writer.Null();
        writer.Null();
    }
    void operator()(const ossia::domain_base<int32_t> & dom)
    {
        if(dom.min) writer.Int(*dom.min); else writer.Null();
        if(dom.max) writer.Int(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Int(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
    void operator()(const ossia::domain_base<float> & dom)
    {
        if(dom.min) writer.Double(*dom.min); else writer.Null();
        if(dom.max) writer.Double(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Double(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::domain_base<char> & dom)
    {
        if(dom.min) writeChar(writer, *dom.min); else writer.Null();
        if(dom.max) writeChar(writer, *dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writeChar(writer, val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::domain_base<bool> & dom)
    {
        if(dom.min) writer.Bool(*dom.min); else writer.Null();
        if(dom.max) writer.Bool(*dom.max); else writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(auto val : dom.values)
                writer.Bool(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::domain_base<std::string> & dom)
    {
        writer.Null();
        writer.Null();

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val : dom.values)
                writer.String(val);
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    void operator()(const ossia::domain_base<std::vector<ossia::value>> & dom)
    {
        if(dom.min)
        {
            writer.StartArray();
            for(auto& val : *dom.min)
                val.apply(value_to_json{writer});
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            writer.StartArray();
            for(auto& val : *dom.max)
                val.apply(value_to_json{writer});
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val : dom.values)
            {
                writer.StartArray();
                for(auto& sub : val)
                    sub.apply(value_to_json{writer});
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }

    template<std::size_t N>
    void operator()(const ossia::domain_base<std::array<float, N>> & dom)
    {
        if(dom.min)
        {
            auto& vec = *dom.min;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            auto& vec = *dom.max;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& vec : dom.values)
            {
                writer.StartArray();
                for(std::size_t i = 0; i < N; i++)
                    writer.Double(vec[i]);
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
    void operator()(const ossia::domain_base<ossia::value> & dom)
    {
        if(dom.min)
        {
            dom.min->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            dom.max->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val: dom.values)
            {
                val.apply(value_to_json{writer});
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
    }
};


/*
struct json_to_domain
{
    const rapidjson::Value& value; // Is an array
    bool operator()()
    {
      return false;
    }

    bool operator()(ossia::domain_base<impulse> & dom)
    {
      return true;
    }

    bool operator()(ossia::domain_base<int32_t> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsInt()) dom.min = arr[0].GetInt(); else dom.min = ossia::none;
          if(arr[1].IsInt()) dom.max = arr[1].GetInt(); else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsInt())
                dom.values.insert(sub.GetInt());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<float> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsNumber()) dom.min = arr[0].GetDouble(); else dom.min = ossia::none;
          if(arr[1].IsNumber()) dom.max = arr[1].GetDouble(); else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsNumber())
                dom.values.insert(sub.GetDouble());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<char> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsString() && arr[0].GetStringLength() > 0) dom.min = arr[0].GetString()[0]; else dom.min = ossia::none;
          if(arr[1].IsString() && arr[1].GetStringLength() > 0) dom.max = arr[1].GetString()[0]; else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsString() && arr[0].GetStringLength() > 0)
                dom.values.insert(sub.GetString()[0]);
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<bool> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 2)
        {
          if(arr[0].IsBool()) dom.min = arr[0].GetBool(); else dom.min = ossia::none;
          if(arr[1].IsBool()) dom.max = arr[1].GetBool(); else dom.max = ossia::none;
        }

        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsBool())
                dom.values.insert(sub.GetBool());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<std::string> & dom)
    {
      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 3)
        {
          if(arr[2].IsArray())
          {
            const auto& values = arr[2].GetArray();
            dom.values.clear();
            dom.values.reserve(values.Size());

            for(const auto& sub : values)
            {
              if(sub.IsString())
                dom.values.insert(sub.GetString());
            }
          }
        }
      }
      return true;
    }

    bool operator()(ossia::domain_base<std::vector<ossia::value>> & dom)
    {
      // Todo : why not instead : RANGE: { min: .. max: .. } or RANGE: [ 0, 4, 6, 123 ]... this would allow more possible domains in the future

      if(value.IsArray())
      {
        const auto& arr = value.GetArray();
        if(arr.Size() >= 3)
        {
          if(arr[0].IsArray())
          {
            // Read the array freely



          }
        }
      }
      return false;
    }

    template<std::size_t N>
    bool operator()(ossia::domain_base<std::array<float, N>> & dom)
    {
      // TODO
        if(dom.min)
        {
            auto& vec = *dom.min;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            auto& vec = *dom.max;
            writer.StartArray();
            for(std::size_t i = 0; i < N; i++)
                writer.Double(vec[i]);
            writer.EndArray();
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& vec : dom.values)
            {
                writer.StartArray();
                for(std::size_t i = 0; i < N; i++)
                    writer.Double(vec[i]);
                writer.EndArray();
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
      return false;
    }

    bool operator()(ossia::domain_base<ossia::value> & dom)
    {
      // TODO
        if(dom.min)
        {
            dom.min->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(dom.max)
        {
            dom.max->apply(value_to_json{writer});
        }
        else
        {
            writer.Null();
        }

        if(!dom.values.empty())
        {
            writer.StartArray();
            for(const auto& val: dom.values)
            {
                val.apply(value_to_json{writer});
            }
            writer.EndArray();
        }
        else
        {
            writer.Null(); // TODO why not just nothing ?
        }
      return false;
    }
};
*/
}
}
}
