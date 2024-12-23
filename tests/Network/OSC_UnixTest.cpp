
#include "AsyncTestUtils.hpp"

#include <ossia/network/context.hpp>

#include "include_catch.hpp"

#include <iostream>

using namespace ossia;

#if defined(OSSIA_PROTOCOL_OSC) && !defined_WIN32
#include <ossia/protocols/osc/osc_factory.hpp>

auto make_unix_client(ossia::net::network_context_ptr ctx)
{
  using conf = ossia::net::osc_protocol_configuration;
  return ossia::net::make_osc_protocol(
      ctx, {conf::MIRROR, conf::OSC1_1, conf::SLIP, conf::NEVER_BUNDLE,
            ossia::net::unix_dgram_configuration{
                {ossia::net::receive_fd_configuration{{"/tmp/ossia_echo.b.socket"}},
                 ossia::net::send_fd_configuration{{"/tmp/ossia_echo.a.socket"}}}}});
}

auto make_unix_server(ossia::net::network_context_ptr ctx)
{
  using conf = ossia::net::osc_protocol_configuration;
  return ossia::net::make_osc_protocol(
      ctx, {conf::HOST, conf::OSC1_1, conf::SLIP, conf::NEVER_BUNDLE,
            ossia::net::unix_dgram_configuration{
                {ossia::net::receive_fd_configuration{{"/tmp/ossia_echo.a.socket"}},
                 ossia::net::send_fd_configuration{{"/tmp/ossia_echo.b.socket"}}}}});
}

TEST_CASE("test_comm_osc_unix_simple", "test_comm_osc_unix_simple")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device server{make_unix_server(ctx), "a"};
  ossia::net::generic_device client{make_unix_client(ctx), "b"};

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

TEST_CASE("test_comm_osc_unix_big", "test_comm_osc_unix_big")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device server{make_unix_server(ctx), "a"};
  ossia::net::generic_device client{make_unix_client(ctx), "b"};

  ossia::value received_from_client;
  ossia::value received_from_server;
  auto on_server_message
      = [&](const std::string& s, const ossia::value& v) { received_from_client = v; };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  auto on_client_message
      = [&](const std::string& s, const ossia::value& v) { received_from_server = v; };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  // Maximum datagram size on macOS (other OSes have larger defaults)
  int n = 1024 - 48;
  const std::string long_str(n, 'x');
  server.get_protocol().push_raw({"/from_server", long_str});
  client.get_protocol().push_raw({"/from_client", long_str});

  ctx->context.run_one();
  ctx->context.run_one();

  REQUIRE(received_from_client == ossia::value{long_str});
  REQUIRE(received_from_server == ossia::value{long_str});
}

TEST_CASE("test_comm_osc_unix", "test_comm_osc_unix")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();

  test_comm_generic_async(
      [=] { return make_unix_server(ctx); }, [=] { return make_unix_client(ctx); },
      *ctx);
}

#endif
