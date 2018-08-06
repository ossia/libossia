#pragma once
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_client.hpp>

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
  template <typename Protocol>
  auto operator()(
      Protocol& proto,
      const typename Protocol::connection_handler& hdl,
      const rapidjson::Document& doc)
  {
    auto& dev = proto.get_device();
    auto& root = proto.get_device().get_root_node();

    // [ { "/addr/val" : 123 } ] or { "/addr/val": 123 }
    if (doc.IsArray())
    {
      const auto& arr = doc.GetArray();
      for (const auto& e : arr)
      {
        json_parser::parse_parameter_value(root, e, dev);
      }
    }
    else
    {
      switch (json_parser::message_type(doc))
      {
        case message_type::Listen:
          handle_listen(proto, hdl, doc);
          break;
        case message_type::Ignore:
          handle_ignore(proto, hdl, doc);
          break;
        default:
          json_parser::parse_parameter_value(root, doc, dev);
          break;
      }
    }
    return rapidjson::StringBuffer{};
  }


  static json_writer::string_t handle_listen(
      oscquery_server_protocol& proto,
      const oscquery_server_protocol::connection_handler& hdl,
      const rapidjson::Document& doc)
  {
    // First we find for a corresponding client
    auto clt = proto.find_client(hdl);

    if (!clt)
      throw bad_request_error{"Client not found"};

    auto m = doc.FindMember("DATA");
    if(m == doc.MemberEnd())
      return {};

    auto nodes = ossia::net::find_nodes(proto.get_device().get_root_node(), m->name.GetString());
    for(auto n : nodes)
    {
      clt->start_listen(n->osc_address(), n->get_parameter());
    }
    return {};
  }

  static json_writer::string_t handle_ignore(
      oscquery_server_protocol& proto,
      const oscquery_server_protocol::connection_handler& hdl,
      const rapidjson::Document& doc)
  {
    // First we find for a corresponding client
    auto clt = proto.find_client(hdl);

    if (!clt)
      throw bad_request_error{"Client not found"};

    auto m = doc.FindMember("DATA");
    if(m == doc.MemberEnd())
      return {};

    auto nodes = ossia::net::find_nodes(proto.get_device().get_root_node(), m->name.GetString());
    for(auto n : nodes)
    {
      clt->stop_listen(n->osc_address());
    }
    return {};
  }
};
}
}
