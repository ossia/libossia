
#include <catch.hpp>
#include <ossia/network/context.hpp>
#include "AsyncTestUtils.hpp"
#include <iostream>

using namespace ossia;


#if defined(OSSIA_PROTOCOL_OSC) && !defined_WIN32
#include <ossia/network/osc/osc_unix.hpp>

TEST_CASE ("test_comm_osc_unix_simple", "test_comm_osc_unix_simple")
{
  using proto = ossia::net::osc_unix_protocol;
  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device server{std::make_unique<proto>(proto::server, ctx, "ossia_test"), "a"};
  ossia::net::generic_device client{std::make_unique<proto>(proto::client, ctx, "ossia_test"), "b"};

  ossia::value received_from_client;
  ossia::value received_from_server;
  auto on_server_message = [&] (const std::string& s, const ossia::value& v) {
    std::cerr << "server: " << s << ": " << v << std::endl;
    received_from_client = v;
  };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  auto on_client_message = [&] (const std::string& s, const ossia::value& v) {
    std::cerr << "client: " << s << ": " << v << std::endl;
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

TEST_CASE ("test_comm_osc_unix_big", "test_comm_osc_unix_big")
{
  using proto = ossia::net::osc_unix_protocol;
  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device server{std::make_unique<proto>(proto::server, ctx, "ossia_test"), "a"};
  ossia::net::generic_device client{std::make_unique<proto>(proto::client, ctx, "ossia_test"), "b"};

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

TEST_CASE ("test_comm_osc_unix", "test_comm_osc_unix")
{
  auto ctx = std::make_shared<ossia::net::network_context>();
  using proto = ossia::net::osc_unix_protocol;
  test_comm_generic_async(
        [=] { return std::make_unique<proto>(proto::server, ctx, "ossia_test"); },
        [=] { return std::make_unique<proto>(proto::client, ctx, "ossia_test"); },
  *ctx);
}

#endif
