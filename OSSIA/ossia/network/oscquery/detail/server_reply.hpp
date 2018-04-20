#pragma once
#include <ossia/detail/json.hpp>

namespace ossia::oscquery
{

struct server_reply
{
  server_reply() = default;
  server_reply(const rapidjson::StringBuffer& str)
  : type{data_type::json}
  , data{str.GetString(), str.GetSize()}
  {

  }
  server_reply(std::string&& str)
    : type{data_type::html}
    , data{std::move(str)}
  {

  }
  enum class data_type { json, html, binary } type;
  std::string data;
};

}
