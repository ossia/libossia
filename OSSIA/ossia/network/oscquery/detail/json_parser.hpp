#pragma once
#include <ossia/network/oscquery/detail/attributes.hpp>
#include <ossia/network/oscquery/detail/json_reader_detail.hpp>
#include <memory>
namespace ossia
{
namespace oscquery
{
//! Functions to parse replies to OSCQuery json requests
struct host_info
{
  enum osc_transport { TCP, UDP };

  std::string name;

  optional<std::string> osc_ip;
  optional<int> osc_port;
  optional<osc_transport> osc_transport;

  optional<std::string> ws_ip;
  optional<int> ws_port;

  ossia::string_map<bool> extensions;
};

struct OSSIA_EXPORT json_parser
{
  static std::shared_ptr<rapidjson::Document>
  parse(const std::string& message);
  static std::shared_ptr<rapidjson::Document>
  parse(const char* data, std::size_t N);

  static int get_port(const rapidjson::Value& obj);

  static ossia::oscquery::message_type
  message_type(const rapidjson::Value& obj);

  static host_info parse_host_info(const rapidjson::Value& obj);
  static void
  parse_namespace(ossia::net::node_base& root, const rapidjson::Value& obj);
  static void
  parse_value(ossia::net::parameter_base& addr, const rapidjson::Value& obj);
  static void parse_parameter_value(
      ossia::net::node_base& root, const rapidjson::Value& obj,
      ossia::net::device_base& dev);
  static void
  parse_path_added(ossia::net::node_base& map, const rapidjson::Value& obj);
  static void
  parse_path_removed(ossia::net::node_base& map, const rapidjson::Value& obj);
  static void
  parse_path_changed(ossia::net::node_base& map, const rapidjson::Value& mess);
  static void parse_attributes_changed(
      ossia::net::node_base& map, const rapidjson::Value& obj);
};
}
}
