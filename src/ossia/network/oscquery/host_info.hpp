#pragma once
#include <ossia/detail/optional.hpp>
#include <ossia/detail/string_map.hpp>
#include <string>

namespace ossia::oscquery
{

//! Represents the capabilities of an OSCQuery server
struct host_info
{
  enum osc_transport
  {
    TCP,
    UDP
  };

  std::string name;

  optional<std::string> osc_ip;
  optional<int> osc_port;
  optional<osc_transport> osc_transport;

  optional<std::string> ws_ip;
  optional<int> ws_port;

  ossia::string_map<bool> extensions;
};

}
