
#include <catch.hpp>
#include <ossia/network/context.hpp>
#include "AsyncTestUtils.hpp"
#include <iostream>

using namespace ossia;


#if defined(OSSIA_PROTOCOL_OSC) && !defined_WIN32
#include <ossia/protocols/osc/osc_generic_protocol.hpp>

TEST_CASE ("test_comm_osc_udp_simple", "test_comm_osc_udp_simple")
{
  using namespace ossia::net;
  using proto = osc_generic_protocol<osc_protocol_client<osc_1_0_policy>, udp_socket>;

  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device server{std::make_unique<proto>(ctx, "0.0.0.0", 4478, "127.0.0.1", 9875), "a"};
  ossia::net::generic_device client{std::make_unique<proto>(ctx, "0.0.0.0", 9875, "127.0.0.1", 4478), "b"};

  ossia::value received_from_client;
  ossia::value received_from_server;
  auto on_server_message = [&] (const std::string& s, const ossia::value& v) {
    received_from_client = v;
  };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  auto on_client_message = [&] (const std::string& s, const ossia::value& v) {
    received_from_server = v;
  };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  server.get_protocol().push_raw({"/from_server", ossia::value{123}});
  client.get_protocol().push_raw({"/from_client", ossia::value{456}});

  ctx->context.run_one();
  ctx->context.run_one();

  REQUIRE(received_from_client ==  ossia::value{456});
  REQUIRE(received_from_server ==  ossia::value{123});
}

TEST_CASE ("test_comm_osc_udp_big", "test_comm_osc_udp_big")
{
  using namespace ossia::net;
  using proto = osc_generic_protocol<osc_protocol_client<osc_1_0_policy>, udp_socket>;

  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device server{std::make_unique<proto>(ctx, "0.0.0.0", 44758, "127.0.0.1", 19875), "a"};
  ossia::net::generic_device client{std::make_unique<proto>(ctx, "0.0.0.0", 19875, "127.0.0.1", 44758), "b"};

  ossia::value received_from_client;
  ossia::value received_from_server;
  auto on_server_message = [&] (const std::string& s, const ossia::value& v) {
    received_from_client = v;
  };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  auto on_client_message = [&] (const std::string& s, const ossia::value& v) {
    received_from_server = v;
  };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  server.get_protocol().push_raw({"/from_server", std::string(60000, 'x')});
  client.get_protocol().push_raw({"/from_client", std::string(60000, 'x')});

  ctx->context.run_one();
  ctx->context.run_one();

  REQUIRE(received_from_client ==  ossia::value{std::string(60000, 'x')});
  REQUIRE(received_from_server ==  ossia::value{std::string(60000, 'x')});
}

TEST_CASE ("test_comm_osc_udp", "test_comm_osc_udp")
{
  using namespace ossia::net;
  using proto = osc_generic_protocol<osc_protocol_client<osc_1_0_policy>, udp_socket>;

  auto ctx = std::make_shared<ossia::net::network_context>();

  test_comm_generic_async(
        [=] { return std::make_unique<proto>(ctx, "0.0.0.0", 55678, "127.0.0.1", 33987); },
        [=] { return std::make_unique<proto>(ctx, "0.0.0.0", 33987, "127.0.0.1", 55678); },
  *ctx);
}

#endif
