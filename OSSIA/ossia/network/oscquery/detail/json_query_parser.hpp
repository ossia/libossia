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
        case message_type::StartOscStreaming:
          handle_start_streaming(proto, hdl, doc);
          break;
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

  static json_writer::string_t handle_start_streaming(
      oscquery_server_protocol& proto,
      const oscquery_server_protocol::connection_handler& hdl,
      const rapidjson::Document& doc)
  {
    auto m = doc.FindMember("DATA");
    if(m == doc.MemberEnd())
      return {};
    if(!m->value.IsObject())
      return {};
    auto obj = m->value.GetObject();
    auto remote_server_port_it = obj.FindMember(detail::local_server_port().data());
    auto remote_sender_port_it = obj.FindMember(detail::local_sender_port().data());

    if(remote_server_port_it == obj.MemberEnd())
      return {};
    if(!remote_server_port_it->value.IsInt())
      return {};

    int remote_server = remote_server_port_it->value.GetInt();
    int remote_sender = 0;

    if(remote_sender_port_it != obj.MemberEnd())
    {
      if(remote_sender_port_it->value.IsInt())
      {
        remote_sender = remote_sender_port_it->value.GetInt();
      }
    }

    return open_osc_sender(proto, hdl, remote_server, remote_sender);
  }

  static json_writer::string_t open_osc_sender(
      oscquery_server_protocol& proto,
      const oscquery_server_protocol::connection_handler& hdl,
      int port,
      int remotePort)
  {
    // First we find for a corresponding client
    auto clt = proto.find_building_client(hdl);

    if (!clt)
    {
      // It's an http-connecting client - we can't open a streaming connection to it
      throw bad_request_error{"Client not found"};
    }

    if(port != 0)
    {
      // Then we set-up the sender
      clt->open_osc_sender(proto.get_logger(), port);
      if(remotePort != 0)
      {
        clt->remote_sender_port = remotePort;
      }
    }

    proto.enable_client(hdl);
    return {};
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

    auto nodes = ossia::net::find_nodes(proto.get_device().get_root_node(), m->value.GetString());
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

    auto nodes = ossia::net::find_nodes(proto.get_device().get_root_node(), m->value.GetString());
    for(auto n : nodes)
    {
      clt->stop_listen(n->osc_address());
    }
    return {};
  }
};
}
}
