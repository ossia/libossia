#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/osc/detail/configuration.hpp>

#include <variant>

namespace ossia::net
{
struct osc_protocol_configuration
{
  // Note: only UDP and UNIX implemented for now

  enum { UDP, UNIX, TCP, SERIAL }   transport{UDP};
  enum { SERVER, CLIENT }           mode{SERVER};
  enum { OSC1_0, OSC1_1, EXTENDED } version{OSC1_0};

  // host: the unix socket name. Pair of sockets will be created in /tmp/
  using unix_configuration = fd_configuration;

  // host: the serial device name
  using serial_configuration = fd_configuration;

  using udp_configuration = socket_configuration;
  using tcp_configuration = socket_configuration;

  std::variant<fd_configuration, socket_configuration> configuration;
};

OSSIA_EXPORT
std::unique_ptr<protocol_base> make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config);
}
