
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
      ctx, {conf::MIRROR, conf::OSC1_1, conf::SIZE_PREFIX, conf::NEVER_BUNDLE,
            ossia::net::unix_stream_configuration{{"/tmp/ossia_echo.server.socket"}}});
}

auto make_unix_server(ossia::net::network_context_ptr ctx)
{
  using conf = ossia::net::osc_protocol_configuration;
  return ossia::net::make_osc_protocol(
      ctx, {conf::HOST, conf::OSC1_1, conf::SIZE_PREFIX, conf::NEVER_BUNDLE,
            ossia::net::unix_stream_configuration{"/tmp/ossia_echo.server.socket"}});
}

TEST_CASE("test_comm_osc_unix_server_client", "test_comm_osc_unix_server_client")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device server{make_unix_server(ctx), "a"};
  ossia::net::generic_device client{make_unix_client(ctx), "b"};

  ctx->context.run_for(std::chrono::milliseconds(100));

  ossia::value received_from_server;
  auto on_client_message
      = [&](const std::string& s, const ossia::value& v) { received_from_server = v; };
  client.on_unhandled_message.connect<decltype(on_client_message)>(on_client_message);

  server.get_protocol().push_raw({"/from_server", ossia::value{123}});

  ctx->context.run_for(std::chrono::milliseconds(100));

  REQUIRE(received_from_server == ossia::value{123});
}

TEST_CASE("test_comm_osc_unix_client_server", "test_comm_osc_unix_client_server")
{
  using namespace ossia::net;

  auto ctx = std::make_shared<ossia::net::network_context>();

  ossia::net::generic_device server{make_unix_server(ctx), "a"};
  ossia::net::generic_device client{make_unix_client(ctx), "b"};

  ctx->context.run_for(std::chrono::milliseconds(100));

  ossia::value received_from_client;
  auto on_server_message
      = [&](const std::string& s, const ossia::value& v) { received_from_client = v; };
  server.on_unhandled_message.connect<decltype(on_server_message)>(on_server_message);

  client.get_protocol().push_raw({"/from_client", ossia::value{456}});

  ctx->context.run_for(std::chrono::milliseconds(100));

  REQUIRE(received_from_client == ossia::value{456});
}
TEST_CASE("test_comm_osc_unix_big", "test_comm_osc_unix_big")
{
  using namespace ossia::net;

  auto ctx1 = std::make_shared<ossia::net::network_context>();
  auto ctx2 = std::make_shared<ossia::net::network_context>();
  ossia::net::generic_device server{make_unix_server(ctx1), "a"};
  ossia::net::generic_device client{make_unix_client(ctx2), "b"};

  ctx1->context.run_for(std::chrono::milliseconds(100));
  ctx2->context.run_for(std::chrono::milliseconds(100));
  std::thread t1{[=] { ctx1->context.run(); }};
  std::thread t2{[=] { ctx2->context.run(); }};

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

  client.get_protocol().push_raw({"/from_client", long_str});
  while(received_from_client.get_type() != ossia::val_type::STRING)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  server.get_protocol().push_raw({"/from_server", long_str});
  while(received_from_server.get_type() != ossia::val_type::STRING)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  REQUIRE(received_from_client == ossia::value{long_str});
  REQUIRE(received_from_server == ossia::value{long_str});

  ctx1->context.stop();
  ctx2->context.stop();
  t1.join();
  t2.join();
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
