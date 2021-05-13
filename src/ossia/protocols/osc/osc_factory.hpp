#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/sockets/configuration.hpp>

#include <variant>

namespace ossia::net
{
struct osc_protocol_configuration
{
  // libossia semantic level: host can change any parameter, mirror can only change BI / SET
  enum { HOST, MIRROR }
  mode{HOST};

  enum { OSC1_0, OSC1_1, EXTENDED }
  version{OSC1_0};

  // Only relevant for stream protocols (TCP, SERIAL, UNIX_STREAM), unused for UDP, UNIX_DGRAM, WEBSOCKETS
  enum { SIZE_PREFIX, SLIP }
  framing{SLIP};

  std::variant<
        udp_configuration
      , tcp_configuration
      , unix_dgram_configuration
      , unix_stream_configuration
      , serial_configuration
      , ws_client_configuration
      , ws_server_configuration
   > transport;
};

using osc_protocol_base = can_learn<protocol_base>;
[[nodiscard]]
OSSIA_EXPORT
std::unique_ptr<osc_protocol_base> make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config);
}
