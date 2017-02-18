#pragma once
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>

namespace ossia
{
namespace oscquery
{

/**
 * @brief OSCQuery JSON query-answering logic.
 *
 * Used by the server to answer client requests.
 */
struct json_query_answerer
{
  template<typename Protocol>
  auto operator()(
      Protocol& proto,
      const typename Protocol::connection_handler&,
      const rapidjson::Document& doc)
  {
      // [ { "/addr/val" : 123 } ] or { "/addr/val": 123 }
      if(doc.IsArray())
      {
        const auto& arr = doc.GetArray();
        for(const auto& e : arr)
        {
          json_parser::parse_address_value(proto.getDevice().getRootNode(), e);
        }
      }
      else
      {
        json_parser::parse_address_value(proto.getDevice().getRootNode(), doc);
      }
      return rapidjson::StringBuffer{};
  }
};
}
}
