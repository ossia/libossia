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

#include <spdlog/sinks/ostream_sink.h>

#include <sstream>
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

namespace
{
// A WebSocket client that opens its connection and resets it at once: with
// read_answer, once the server has answered the handshake
void open_and_reset(int port, bool read_answer)
{
  boost::asio::io_context ctx;
  boost::asio::ip::tcp::socket sock{ctx};
  sock.connect({boost::asio::ip::make_address("127.0.0.1"), (unsigned short)port});
  const std::string handshake
      = "GET / HTTP/1.1\r\nHost: 127.0.0.1:" + std::to_string(port)
        + "\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n"
          "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n";
  boost::asio::write(sock, boost::asio::buffer(handshake));
  if(read_answer)
  {
    std::string answer;
    char buf[512];
    boost::system::error_code ec;
    while(answer.find("\r\n\r\n") == std::string::npos)
    {
      auto n = sock.read_some(boost::asio::buffer(buf), ec);
      if(ec || n == 0)
        break;
      answer.append(buf, n);
    }
    REQUIRE(answer.find(" 101 ") != std::string::npos);
  }
  sock.set_option(boost::asio::socket_base::linger{true, 0});
  sock.close();
}

// Clients that leave as soon as they are connected (a mirror stopped as it
// connects, a port scan) are no error; a client that stays still connects
template <typename F>
std::string log_of(F&& f)
{
  std::ostringstream log;
  auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(log);
  ossia::logger().sinks().push_back(sink);
  f();
  ossia::logger().sinks().pop_back();
  return log.str();
}

void clients_that_leave(int ws_port)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  for(int i = 0; i < 200; i++)
    open_and_reset(ws_port, i % 2 == 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  ossia_test::raw_ws_client c{ws_port};
  REQUIRE(c.connected());
}
}

TEST_CASE("test_oscquery_client_leaves_at_once", "test_oscquery_client_leaves_at_once")
{
  auto log = log_of([] {
    generic_device dev{
        std::make_unique<ossia::oscquery::oscquery_server_protocol>(1724, 5724), "A"};
    clients_that_leave(5724);
  });
  INFO(log);
  REQUIRE(log.find("on_connectionOpen") == std::string::npos);
}

TEST_CASE(
    "test_oscquery_asio_client_leaves_at_once",
    "test_oscquery_asio_client_leaves_at_once")
{
  auto log = log_of([] {
    auto ctx = std::make_shared<ossia::net::network_context>();
    generic_device dev{
        std::make_unique<ossia::oscquery_asio::oscquery_server_protocol>(
            ctx, 1725, 5725),
        "A"};
    std::thread t{[ctx] { ctx->run(); }};
    clients_that_leave(5725);
    ctx->context.stop();
    t.join();
  });
  INFO(log);
  REQUIRE(log.find("on_connectionOpen") == std::string::npos);
}
