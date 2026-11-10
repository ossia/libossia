#pragma once
#include <ossia/detail/variant.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/sockets/configuration.hpp>

namespace ossia::net
{
struct oscquery_server_protocol_configuration
{
  //! Which framing mechanism to use for the query information (the JSON
  //! messages)
  framing query_framing{framing::slip};

  //! Which transport is used for the query information (the JSON messages)
  ossia::variant<
      ws_server_configuration, tcp_configuration, unix_stream_configuration,
      serial_configuration>
      query_transport;

  using declared_transport = ossia::variant<
      udp_receiver_configuration, tcp_configuration, unix_dgram_configuration,
      unix_stream_configuration, serial_configuration, ws_server_configuration>;

  //! Which open hosts/ports will be advertised by this server in the HOST_INFO
  //! query for receiving OSC messages.
  std::vector<declared_transport> declared_transports;
};

[[nodiscard]] OSSIA_EXPORT std::unique_ptr<protocol_base> make_oscquery_server_protocol(
    network_context_ptr ctx, oscquery_server_protocol_configuration config);
}
