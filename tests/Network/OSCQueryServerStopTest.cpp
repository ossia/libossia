// What an OSCQuery server leaves behind when it stops
#include "OSCQueryHttpTestUtils.hpp"
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/zeroconf/zeroconf.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include <thread>

using namespace ossia;
using namespace ossia::net;

namespace
{
// Whether a device of that name is announced over mDNS (browses 5 s)
bool announced(const std::string& name)
{
  return ossia::any_of(ossia::net::list_oscquery_devices(), [&](const auto& d) {
    return d.name == name;
  });
}

std::string unique_name(const char* what)
{
  return std::string{"ossia_test_"} + what + "_"
         + std::to_string(
             std::chrono::steady_clock::now().time_since_epoch().count() % 1000000);
}
}

TEST_CASE(
    "test_oscquery_stop_withdraws_announcement",
    "test_oscquery_stop_withdraws_announcement")
{
  if(!ossia::net::zeroconf_supported())
    return;

  // A server stopped, not destroyed, is no longer announced
  const auto name = unique_name("stop");
  auto proto = new ossia::oscquery::oscquery_server_protocol{1720, 5720};
  generic_device dev{std::unique_ptr<protocol_base>(proto), name};
  if(!announced(name))
    SKIP("this machine does not see its own mDNS announcements");
  proto->stop();
  REQUIRE(!announced(name));
}

TEST_CASE(
    "test_oscquery_asio_stop_withdraws_announcement",
    "test_oscquery_asio_stop_withdraws_announcement")
{
  if(!ossia::net::zeroconf_supported())
    return;

  const auto name = unique_name("asio_stop");
  auto ctx = std::make_shared<ossia::net::network_context>();
  auto proto = new ossia::oscquery_asio::oscquery_server_protocol{ctx, 1721, 5721};
  generic_device dev{std::unique_ptr<protocol_base>(proto), name};
  std::thread t{[ctx] { ctx->run(); }};
  // The context thread ends however the test does
  struct joiner
  {
    std::shared_ptr<ossia::net::network_context>& ctx;
    std::thread& t;
    ~joiner()
    {
      ctx->context.stop();
      if(t.joinable())
        t.join();
    }
  } join{ctx, t};
  if(!announced(name))
    SKIP("this machine does not see its own mDNS announcements");
  proto->stop();
  REQUIRE(!announced(name));
}

TEST_CASE(
    "test_oscquery_asio_destroyed_with_a_client",
    "test_oscquery_asio_destroyed_with_a_client")
{
  // A server destroyed while a client is connected: the closing of that
  // connection, run by a later poll of the context, reaches nothing
  auto ctx = std::make_shared<ossia::net::network_context>();
  auto dev = std::make_unique<generic_device>(
      std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(ctx, 1722, 5722),
      "A");
  find_or_create_node(*dev, "/foo").create_parameter(val_type::FLOAT);

  std::atomic_bool polling{true};
  std::thread poller{[&] {
    while(polling)
    {
      ctx->context.poll();
      ctx->context.restart();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }};
  auto c = std::make_unique<ossia_test::raw_ws_client>(5722);
  REQUIRE(c->connected());
  polling = false;
  poller.join();

  dev.reset();
  c.reset();
  for(int i = 0; i < 50; i++)
  {
    REQUIRE_NOTHROW(ctx->context.poll());
    ctx->context.restart();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

TEST_CASE(
    "test_oscquery_asio_stopped_with_a_client",
    "test_oscquery_asio_stopped_with_a_client")
{
  // Stopped and kept alive: the handlers do not read the WebSocket server
  // that stop() reset
  auto ctx = std::make_shared<ossia::net::network_context>();
  auto proto = new ossia::oscquery_asio::oscquery_server_protocol{ctx, 1723, 5723};
  generic_device dev{std::unique_ptr<protocol_base>(proto), "A"};
  std::thread t{[ctx] { ctx->run(); }};
  {
    ossia_test::raw_ws_client c{5723};
    REQUIRE(c.connected());
    proto->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  ctx->context.stop();
  t.join();
}
