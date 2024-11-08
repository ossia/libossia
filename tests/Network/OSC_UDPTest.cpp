
#include "AsyncTestUtils.hpp"

#include <ossia/network/context.hpp>
#include <ossia/network/sockets/udp_socket.hpp>

#include "include_catch.hpp"

#include <iostream>

using namespace ossia;

#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/protocols/osc/osc_generic_protocol.hpp>

using conf = ossia::net::udp_configuration;
using recv_sock = ossia::net::inbound_socket_configuration;
using send_sock = ossia::net::outbound_socket_configuration;
static const conf server_conf
    = conf{{recv_sock{"0.0.0.0", 4478}, send_sock{"127.0.0.1", 9875}}};
static const conf client_conf
    = conf{{recv_sock{"0.0.0.0", 9875}, send_sock{"127.0.0.1", 4478}}};
TEST_CASE("test_comm_osc_udp_simple", "test_comm_osc_udp_simple")
{
  using namespace ossia::net;
  using proto = osc_generic_bidir_protocol<
      osc_protocol_client<osc_1_0_policy>, udp_send_socket, udp_receive_socket>;

  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device server{
      std::make_unique<proto>(ctx, *server_conf.remote, *server_conf.local), "a"};
  ossia::net::generic_device client{
      std::make_unique<proto>(ctx, *client_conf.remote, *client_conf.local), "b"};

  ossia::value received_from_client;
  ossia::value received_from_server;
  auto on_server_message
      = [&](const std::string& s, const ossia::value& v) { received_from_client = v; };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  auto on_client_message
      = [&](const std::string& s, const ossia::value& v) { received_from_server = v; };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  server.get_protocol().push_raw({"/from_server", ossia::value{123}});
  client.get_protocol().push_raw({"/from_client", ossia::value{456}});

  ctx->context.run_one();
  ctx->context.run_one();

  REQUIRE(received_from_client == ossia::value{456});
  REQUIRE(received_from_server == ossia::value{123});
}

TEST_CASE("test_comm_osc_udp_big", "test_comm_osc_udp_big")
{
  using namespace ossia::net;
  using proto = osc_generic_bidir_protocol<
      osc_protocol_client<osc_1_0_policy>, udp_send_socket, udp_receive_socket>;

  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device server{
      std::make_unique<proto>(ctx, *server_conf.remote, *server_conf.local), "a"};
  ossia::net::generic_device client{
      std::make_unique<proto>(ctx, *client_conf.remote, *client_conf.local), "b"};

  ossia::value received_from_client;
  ossia::value received_from_server;
  auto on_server_message
      = [&](const std::string& s, const ossia::value& v) { received_from_client = v; };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  auto on_client_message
      = [&](const std::string& s, const ossia::value& v) { received_from_server = v; };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  // Maximum datagram size on macOS (other OSes have larger defaults)
  int n = 9216 - 48;
  const std::string long_str(n, 'x');
  server.get_protocol().push_raw({"/from_server", long_str});
  client.get_protocol().push_raw({"/from_client", long_str});

  ctx->context.run_one();
  ctx->context.run_one();

  REQUIRE(received_from_client == ossia::value{long_str});
  REQUIRE(received_from_server == ossia::value{long_str});
}

TEST_CASE("test_comm_osc_udp", "test_comm_osc_udp")
{
  using namespace ossia::net;
  using namespace ossia::net;
  using proto = osc_generic_bidir_protocol<
      osc_protocol_client<osc_1_0_policy>, udp_send_socket, udp_receive_socket>;

  auto ctx = std::make_shared<ossia::net::network_context>();

  test_comm_generic_async(
      [=] {
    return std::make_unique<proto>(ctx, *server_conf.remote, *server_conf.local);
      },
      [=] {
    return std::make_unique<proto>(ctx, *client_conf.remote, *client_conf.local);
  },
      *ctx);
}

#endif
