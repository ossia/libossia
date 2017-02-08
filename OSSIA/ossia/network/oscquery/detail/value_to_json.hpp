#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/detail/json.hpp>
#include <oscpack/osc/OscTypes.h>

namespace ossia
{
namespace oscquery
{
namespace detail
{

// TODO base64 encode
struct value_to_json
{
  rapidjson::Writer<rapidjson::StringBuffer>& writer;
  void operator()(impulse) const { writer.Null(); }
  void operator()(int v) const { writer.Int(v); }
  void operator()(float v) const { writer.Double(v); }
  void operator()(bool v) const { writer.Bool(v); }
  void operator()(char v) const { writeChar(writer, v); }
  void operator()(const std::string& v) const
  {
    // TODO handle base 64
    // bool b = Base64::Encode(get<coppa::Generic>(val).buf, &out);
    writer.String(v);
  }

  template<std::size_t N>
  void operator()(const std::array<float, N>& vec) const {
    writer.StartArray();
    for(std::size_t i = 0; i < N; i++)
    {
      writer.Double(vec[i]);
    }
    writer.EndArray();
  }

  void operator()(const std::vector<ossia::value>& vec) const
  {
    writer.StartArray();
    for(const auto& sub : vec)
    {
      sub.apply(*this);
    }
    writer.EndArray();
  }

  void operator()(const ossia::Destination& d) const
  {
    throw;
  }


  void operator()() const
  {
    throw;
  }
};

struct json_to_value
{
  const rapidjson::Value& val;
  ossia::string_view& typetags;
  int& typetag_cursor;
  bool operator()(impulse) const
  {
    typetag_cursor++;
    return val.IsNull();
  }

  bool operator()(int& res) const
  {
    typetag_cursor++;

    bool b = val.IsInt();
    if(b)
      res = val.GetInt();
    return b;
  }

  bool operator()(float &res) const
  {
    typetag_cursor++;

    bool b = val.IsDouble();
    if(b)
      res = val.GetDouble();
    return b;
  }

  bool operator()(bool &res) const
  {
    typetag_cursor++;

    bool b = val.IsBool();
    if(b)
      res = val.GetBool();
    return b;
  }

  bool operator()(char &res) const
  {
    typetag_cursor++;

    bool b = val.IsString() && val.GetStringLength() > 0;
    if(b)
      res = val.GetString()[0];
    return b;
  }

  bool operator()(std::string& res) const
  {
    typetag_cursor++;
    // TODO handle base 64
    // bool b = Base64::Encode(get<coppa::Generic>(val).buf, &out);

    bool b = val.IsString();
    if(b)
      res = std::string(val.GetString(), val.GetStringLength());
    return b;
  }

  template<std::size_t N>
  bool operator()(std::array<float, N>& res) const
  {
    typetag_cursor += N;
    bool b = val.IsArray();
    if(b)
    {
      auto arr = val.GetArray();
      if(arr.Size() == N)
      {
        for(int i = 0; i < (int)N; i++)
        {
          if(arr[i].IsDouble())
          {
            res[i] = arr[i].GetDouble();
          }
          else
          {
            b = false;
            break;
          }
        }
      }
      else
      {
        b = false;
      }
    }
    return b;
  }

  bool handleVecElement(const rapidjson::Value& elt, std::vector<ossia::value>& res) const
  {
    if((int)typetags.size() > typetag_cursor)
    {
      switch(typetags[typetag_cursor])
      {
        case oscpack::TypeTagValues::INFINITUM_TYPE_TAG:
        {
          ossia::impulse i;
          if(!json_to_value{elt, typetags, typetag_cursor}(i))
            return false;

          res.push_back(i);
          return true;
        }
        case oscpack::TypeTagValues::INT32_TYPE_TAG:
        {
          int32_t i{};
          if(!json_to_value{elt, typetags, typetag_cursor}(i))
            return false;

          res.push_back(i);
          return true;
        }
        case oscpack::TypeTagValues::FLOAT_TYPE_TAG:
        {
          float i{};
          if(!json_to_value{elt, typetags, typetag_cursor}(i))
            return false;

          res.push_back(i);
          return true;
        }
        case oscpack::TypeTagValues::CHAR_TYPE_TAG:
        {
          char i{};
          if(!json_to_value{elt, typetags, typetag_cursor}(i))
            return false;

          res.push_back(i);
          return true;
        }

        case oscpack::TypeTagValues::TRUE_TYPE_TAG:
        case oscpack::TypeTagValues::FALSE_TYPE_TAG:
        {
          bool i{};
          if(!json_to_value{elt, typetags, typetag_cursor}(i))
            return false;

          res.push_back(i);
          return true;
        }

        case oscpack::TypeTagValues::STRING_TYPE_TAG:
        case oscpack::TypeTagValues::SYMBOL_TYPE_TAG:
        {
          std::string i;
          if(!json_to_value{elt, typetags, typetag_cursor}(i))
            return false;

          res.push_back(std::move(i));
          return true;
        }

        case oscpack::TypeTagValues::ARRAY_BEGIN_TYPE_TAG:
        {
          std::vector<ossia::value> i;
          ++typetag_cursor; // We skip the '['
          if(!json_to_value{elt, typetags, typetag_cursor}(i))
            return false;

          ++typetag_cursor; // We skip the ']'
          res.push_back(std::move(i));
          return true;
        }
        case oscpack::TypeTagValues::ARRAY_END_TYPE_TAG:
        default:
        {
          // We should never end up here
          return false;
        }
      }

    }
    else
    {
      return false;
    }

  }

  bool operator()(std::vector<ossia::value>& res) const
  {
    // TODO read from the typetag
    bool b = val.IsArray();
    if(b)
    {
      auto arr = val.GetArray();
      for(const auto& elt : arr)
      {
        if(!handleVecElement(elt, res))
          return false;
      }
    }
    return b;
  }

  bool operator()(ossia::Destination& d) const
  {
    throw;
  }

  bool operator()() const
  {
    throw;
  }
};

}
}
}
