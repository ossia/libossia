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

  std::optional<std::string> osc_ip;
  std::optional<int> osc_port;
  std::optional<osc_transport> osc_transport;

  std::optional<std::string> ws_ip;
  std::optional<int> ws_port;

  ossia::string_map<bool> extensions;
};

}
