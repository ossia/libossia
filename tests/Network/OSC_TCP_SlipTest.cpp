
#include "AsyncTestUtils.hpp"

#include <ossia/network/context.hpp>

#include "include_catch.hpp"

#include <iostream>

using namespace ossia;

#if defined(OSSIA_PROTOCOL_OSC)
#include <ossia/protocols/osc/osc_factory.hpp>

auto make_client(ossia::net::network_context_ptr ctx)
{
  using conf = ossia::net::osc_protocol_configuration;
  return ossia::net::make_osc_protocol(
      ctx, {conf::MIRROR, conf::OSC1_1, conf::SLIP, conf::NEVER_BUNDLE,
            ossia::net::tcp_client_configuration{"127.0.0.1", 1234}});
}

auto make_server(ossia::net::network_context_ptr ctx)
{
  using conf = ossia::net::osc_protocol_configuration;
  return ossia::net::make_osc_protocol(
      ctx, {conf::HOST, conf::OSC1_1, conf::SLIP, conf::NEVER_BUNDLE,
            ossia::net::tcp_server_configuration{"0.0.0.0", 1234}});
}

TEST_CASE("test_comm_osc_tcp_server_client", "test_comm_osc_tcp_server_client")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device server{make_server(ctx), "a"};
  ossia::net::generic_device client{make_client(ctx), "b"};

  ctx->context.run_for(std::chrono::milliseconds(100));

  ossia::value received_from_server;
  auto on_client_message
      = [&](const std::string& s, const ossia::value& v) { received_from_server = v; };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  server.get_protocol().push_raw({"/from_server", ossia::value{123}});

  ctx->context.run_for(std::chrono::milliseconds(100));

  REQUIRE(received_from_server == ossia::value{123});
}

TEST_CASE("test_comm_osc_tcp_client_server", "test_comm_osc_tcp_client_server")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device server{make_server(ctx), "a"};
  ossia::net::generic_device client{make_client(ctx), "b"};

  ctx->context.run_for(std::chrono::milliseconds(100));

  ossia::value received_from_client;
  auto on_server_message
      = [&](const std::string& s, const ossia::value& v) { received_from_client = v; };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  client.get_protocol().push_raw({"/from_client", ossia::value{456}});

  ctx->context.run_for(std::chrono::milliseconds(100));

  REQUIRE(received_from_client == ossia::value{456});
}
TEST_CASE("test_comm_osc_tcp_big", "test_comm_osc_tcp_big")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device server{make_server(ctx), "a"};
  ossia::net::generic_device client{make_client(ctx), "b"};

  ctx->context.run_for(std::chrono::milliseconds(100));

  ossia::value received_from_client;
  ossia::value received_from_server;
  auto on_server_message
      = [&](const std::string& s, const ossia::value& v) { received_from_client = v; };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  auto on_client_message
      = [&](const std::string& s, const ossia::value& v) { received_from_server = v; };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  int n = std::pow(2, 15);
  const std::string long_str(n, 'x');
  server.get_protocol().push_raw({"/from_server", long_str});
  client.get_protocol().push_raw({"/from_client", long_str});

  ctx->context.run_for(std::chrono::milliseconds(100));

  REQUIRE(received_from_client == ossia::value{long_str});
  REQUIRE(received_from_server == ossia::value{long_str});
}

TEST_CASE("test_comm_osc_tcp", "test_comm_osc_tcp")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();

  test_comm_generic_async(
      [=] { return make_server(ctx); }, [=] { return make_client(ctx); }, *ctx);
}

#endif
