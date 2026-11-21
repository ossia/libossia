// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/sockets/websocket_server.hpp>
#include <ossia/network/value/value_conversion.hpp>

#include <oscpack/osc/OscOutboundPacketStream.h>

#include "include_catch.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

namespace
{
// Each setup takes its own ports, in 1790-1799, 5790-5799, 10790-10799
int next_index = 0;

template <typename F>
bool wait_for(F f, std::chrono::milliseconds timeout = 2000ms)
{
  const auto end = std::chrono::steady_clock::now() + timeout;
  while(std::chrono::steady_clock::now() < end)
  {
    if(f())
      return true;
    std::this_thread::sleep_for(5ms);
  }
  return f();
}

// A server of the test, with a float /level at 0.3, that answers its HTTP
// value reads with http_value, http_delay late, its reads after LISTEN as
// listen_answer says, and sends values when it is told to
struct slow_server
{
  enum class listen_answer
  {
    answer,  // with 0.8
    silent,  // nothing
    overtake // 0.9 as a value first, then 0.8 as the answer
  };
  std::mutex mutex;
  ossia::net::websocket_server::connection_handler client;
  std::atomic_bool connected{false};
  std::atomic_int value_reads{0};
  std::atomic_int listen_reads{0};
  std::chrono::milliseconds http_delay{300};
  float http_value{100.f};
  listen_answer on_listen_read{listen_answer::answer};
  ossia::net::websocket_server server;
  std::thread thread;

  explicit slow_server(
      int port, std::chrono::milliseconds delay = 300ms, float value = 100.f,
      listen_answer listen = listen_answer::answer)
      : http_delay{delay}
      , http_value{value}
      , on_listen_read{listen}
  {
    server.set_open_handler([this](auto hdl) {
      std::lock_guard l{mutex};
      client = hdl;
      connected = true;
    });
    server.set_close_handler([](auto) { });
    server.set_message_handler(
        [this](auto, auto, const std::string& resource) -> ossia::net::server_reply {
      if(resource.find("HOST_INFO") != std::string::npos)
        return reply(R"({"NAME":"slow","EXTENSIONS":{"TYPE":true,"VALUE":true}})");
      if(resource == "/")
        return reply(
            R"({"FULL_PATH":"/","CONTENTS":{"level":{"FULL_PATH":"/level","TYPE":"f","VALUE":[0.3],"ACCESS":3}}})");
      if(resource == "/level?VALUE")
      {
        value_reads++;
        std::this_thread::sleep_for(http_delay);
        return reply("{\"VALUE\":" + std::to_string(http_value) + "}");
      }
      if(resource == "/level?VALUE&FULL_PATH")
      {
        listen_reads++;
        if(on_listen_read == listen_answer::silent)
          return reply("");
        if(on_listen_read == listen_answer::overtake)
          send_level(0.9f);
        return reply(R"({"FULL_PATH":"/level","VALUE":0.8})");
      }
      // A WebSocket command of the mirror (LISTEN...)
      return reply("");
    });
    server.listen(port);
    thread = std::thread{[this] { server.run(); }};
  }
  ~slow_server()
  {
    server.stop();
    thread.join();
  }
  static ossia::net::server_reply reply(const std::string& json)
  {
    ossia::net::server_reply r;
    r.type = ossia::net::server_reply::data_type::json;
    r.data = json;
    return r;
  }
  // A value of /level, as OSC over the WebSocket
  void send_level(float v)
  {
    char buf[256];
    oscpack::OutboundPacketStream p(buf, sizeof(buf));
    p << oscpack::BeginMessage("/level") << v << oscpack::EndMessage();
    std::lock_guard l{mutex};
    server.send_binary_message(client, std::string_view{p.Data(), p.Size()});
  }
};

// A server that does not answer crossed writes, and a mirror of it
struct setup
{
  const int index = next_index++ % 10;
  generic_device server{
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(
          1790 + index, 5790 + index),
      "crossing_server"};
  parameter_base& level
      = *create_node(server.get_root_node(), "/level").create_parameter(val_type::FLOAT);

  ossia::oscquery::oscquery_mirror_protocol* proto{};
  std::unique_ptr<generic_device> mirror;
  parameter_base* remote{};
  std::mutex mutex;
  std::vector<float> received;

  explicit setup(bool reread)
  {
    proto = new ossia::oscquery::oscquery_mirror_protocol(
        "ws://127.0.0.1:" + std::to_string(5790 + index), uint16_t(10790 + index));
    proto->set_reread_crossings(reread);
    mirror = std::make_unique<generic_device>(
        std::unique_ptr<protocol_base>(proto), "crossing_mirror");
    proto->update(mirror->get_root_node());
    auto node = find_node(mirror->get_root_node(), "/level");
    REQUIRE(node);
    remote = node->get_parameter();
    REQUIRE(remote);
    // What comes from the server: the mirror's own writes go through
    // push_value_quiet and call no callback
    remote->add_callback([this](const ossia::value& v) {
      std::lock_guard lock{mutex};
      received.push_back(ossia::convert<float>(v));
    });
    std::this_thread::sleep_for(200ms);
  }

  std::vector<float> values()
  {
    std::lock_guard lock{mutex};
    return received;
  }

  // The server and the client write the same address in the same instant,
  // 20 times: how many times they end apart
  int crossed_writes()
  {
    int divergences = 0;
    for(int i = 0; i < 20; i++)
    {
      std::atomic_bool go{false};
      std::thread client([&] {
        while(!go)
        {
        }
        remote->push_value_quiet(200.f + i);
      });
      go = true;
      level.push_value(100.f + i);
      client.join();
      std::this_thread::sleep_for(600ms);
      const float at_server = ossia::convert<float>(level.value());
      const float at_client = ossia::convert<float>(remote->value());
      if(at_server != at_client)
      {
        divergences++;
        UNSCOPED_INFO(
            "trial " << i << ": server " << at_server << ", client " << at_client);
      }
    }
    return divergences;
  }
};
}

TEST_CASE("test_oscquery_mirror_crossings_off", "[oscquery]")
{
  setup s{false};
  REQUIRE(!s.proto->get_reread_crossings());
  s.crossed_writes();
  // As before: the mirror reads nothing again, and gets only what the server
  // sends on its own
  for(float v : s.values())
    CHECK(v < 200.f);
}

TEST_CASE("test_oscquery_mirror_reads_again_after_a_crossing", "[oscquery]")
{
  setup s{true};
  CHECK(s.crossed_writes() == 0);
}

TEST_CASE("test_oscquery_mirror_no_crossing_no_read", "[oscquery]")
{
  // A value that comes long after the client's write crossed nothing: it is
  // not read again
  setup s{true};
  s.remote->push_value_quiet(5.f);
  REQUIRE(wait_for([&] { return ossia::convert<float>(s.level.value()) == 5.f; }));
  std::this_thread::sleep_for(
      ossia::oscquery::oscquery_mirror_protocol::crossing_window + 100ms);
  s.level.push_value(1.f);
  REQUIRE(wait_for([&] { return !s.values().empty(); }));
  std::this_thread::sleep_for(600ms);
  CHECK(s.values() == std::vector<float>{1.f});
}

TEST_CASE("test_oscquery_mirror_read_again_unchanged", "[oscquery]")
{
  // The server's value comes after the client's write, and differs from it:
  // it is read again, and the server answers the value it already sent.
  // What follows the parameter gets it once.
  setup s{true};
  s.remote->push_value_quiet(200.f);
  REQUIRE(wait_for([&] { return ossia::convert<float>(s.level.value()) == 200.f; }));
  s.level.push_value(100.f);
  REQUIRE(wait_for([&] { return !s.values().empty(); }));
  std::this_thread::sleep_for(
      ossia::oscquery::oscquery_mirror_protocol::settle_delay + 300ms);
  CHECK(s.values() == std::vector<float>{100.f});
}

TEST_CASE("test_oscquery_mirror_read_older_than_a_write", "[oscquery]")
{
  // The read after a crossing is answered after the client wrote again: the
  // answer is older than that write and does not undo it
  slow_server server{5799};
  auto proto = new ossia::oscquery::oscquery_mirror_protocol(
      "ws://127.0.0.1:5799", uint16_t(10799));
  proto->set_reread_crossings(true);
  generic_device mirror{std::unique_ptr<protocol_base>(proto), "slow_mirror"};
  proto->update(mirror.get_root_node());
  auto node = find_node(mirror.get_root_node(), "/level");
  REQUIRE(node);
  auto remote = node->get_parameter();
  REQUIRE(remote);
  std::mutex mutex;
  std::vector<float> received;
  remote->add_callback([&](const ossia::value& v) {
    std::lock_guard lock{mutex};
    received.push_back(ossia::convert<float>(v));
  });
  REQUIRE(wait_for([&] { return server.connected.load(); }));

  remote->push_value_quiet(200.f);
  // Crosses the write: read again settle_delay later
  server.send_level(100.f);
  REQUIRE(wait_for([&] { return server.value_reads > 0; }));
  remote->push_value_quiet(300.f);
  std::this_thread::sleep_for(600ms);

  CHECK(ossia::convert<float>(remote->value()) == 300.f);
  std::lock_guard lock{mutex};
  CHECK(received == std::vector<float>{100.f});
}

namespace
{
// A mirror of a slow_server, that reads after LISTEN or not, and follows
// /level
struct listen_mirror
{
  ossia::oscquery::oscquery_mirror_protocol* proto;
  generic_device dev;
  parameter_base* level{};
  std::mutex mutex;
  std::vector<float> received;

  listen_mirror(slow_server& server, int port, bool read_after_listen)
      : proto{new ossia::oscquery::oscquery_mirror_protocol(
            "ws://127.0.0.1:" + std::to_string(port), uint16_t(port + 5000))}
      , dev{std::unique_ptr<protocol_base>(proto), "listen_mirror"}
  {
    proto->set_read_after_listen(read_after_listen);
    proto->update(dev.get_root_node());
    auto node = find_node(dev.get_root_node(), "/level");
    REQUIRE(node);
    level = node->get_parameter();
    REQUIRE(level);
    REQUIRE(wait_for([&] { return server.connected.load() && proto->connected(); }));
    // The LISTEN, and the read after it
    level->add_callback([this](const ossia::value& v) {
      std::lock_guard lock{mutex};
      received.push_back(ossia::convert<float>(v));
    });
  }
  std::vector<float> values()
  {
    std::lock_guard lock{mutex};
    return received;
  }
  float value() const { return ossia::convert<float>(level->value()); }
};
}

TEST_CASE("test_oscquery_mirror_read_after_listen_off", "[oscquery]")
{
  // As before: nothing is read after LISTEN, and the value the server
  // changed before it applied the LISTEN is not known
  slow_server server{5798};
  listen_mirror m{server, 5798, false};
  REQUIRE(!m.proto->get_read_after_listen());
  std::this_thread::sleep_for(500ms);
  CHECK(server.listen_reads == 0);
  CHECK(m.value() == 0.3f);
  CHECK(m.values().empty());
}

TEST_CASE("test_oscquery_mirror_read_after_listen", "[oscquery]")
{
  slow_server server{5797};
  listen_mirror m{server, 5797, true};
  REQUIRE(wait_for([&] { return m.value() == 0.8f; }));
  CHECK(server.listen_reads == 1);
  CHECK(
      m.proto->get_listen_read_mode()
      == ossia::oscquery::oscquery_mirror_protocol::listen_read_mode::websocket);
  std::this_thread::sleep_for(300ms);
  CHECK(m.values() == std::vector<float>{0.8f});
}

TEST_CASE("test_oscquery_mirror_read_after_listen_overtaken", "[oscquery]")
{
  // A value of the parameter comes before the answer: it is newer, and the
  // answer is dropped
  slow_server server{5796, 300ms, 100.f, slow_server::listen_answer::overtake};
  listen_mirror m{server, 5796, true};
  REQUIRE(wait_for([&] { return server.listen_reads == 1; }));
  std::this_thread::sleep_for(500ms);
  CHECK(m.value() == 0.9f);
  CHECK(m.values() == std::vector<float>{0.9f});
}

TEST_CASE("test_oscquery_mirror_read_after_listen_over_http", "[oscquery]")
{
  // A server that answers nothing on the WebSocket: the read is sent again
  // over HTTP after listen_read_probe
  slow_server server{5795, 0ms, 0.7f, slow_server::listen_answer::silent};
  listen_mirror m{server, 5795, true};
  REQUIRE(wait_for([&] {
    return m.value() == 0.7f;
  }, ossia::oscquery::oscquery_mirror_protocol::listen_read_probe + 1000ms));
  CHECK(server.value_reads == 1);
  CHECK(
      m.proto->get_listen_read_mode()
      == ossia::oscquery::oscquery_mirror_protocol::listen_read_mode::http);
  std::this_thread::sleep_for(300ms);
  CHECK(m.values() == std::vector<float>{0.7f});
}
