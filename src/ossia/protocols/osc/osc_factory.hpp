#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/sockets/configuration.hpp>

#include <variant>

namespace ossia::net
{
struct osc_protocol_configuration
{
  enum { UDP, UNIX_DGRAM, TCP, UNIX_STREAM, SERIAL, WEBSOCKETS }
  transport{UDP};

  // libossia semantic level: host can change any parameter, mirror can only change BI / SET
  enum { HOST, MIRROR }
  mode{HOST};

  enum { OSC1_0, OSC1_1, EXTENDED }
  version{OSC1_0};

  // Only relevant for stream protocols (TCP, SERIAL, UNIX_STREAM), unused for UDP, UNIX_DGRAM, WEBSOCKETS
  enum { SIZE_PREFIX, SLIP }
  framing{SLIP};

  std::variant<
        fd_configuration
      , socket_configuration
      , double_fd_configuration
      , double_socket_configuration
      , serial_configuration
      , ws_client_configuration
      , ws_server_configuration
   > configuration;
};

using osc_protocol_base = can_learn<protocol_base>;
OSSIA_EXPORT
std::unique_ptr<osc_protocol_base> make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config);
}
