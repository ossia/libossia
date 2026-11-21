// A mirror and a burst of PATH_CHANGED from a server slow to answer: with
// set_request_queues, the readings do not pile up
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

namespace
{
// A server of the test that serves a fixed tree, takes 100 ms to read /a,
// counts the readings of /a, and can send a text message to its client
struct slow_server
{
  std::mutex mutex;
  ossia::net::websocket_server::connection_handler client;
  std::atomic_bool connected{false};
  std::atomic_int readings{0};
  ossia::net::websocket_server server;
  std::thread thread;

  explicit slow_server(int port)
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
        return reply(
            R"({"NAME":"slow","EXTENSIONS":{"TYPE":true,"VALUE":true,"PATH_CHANGED":true,"PATH_ADDED":true,"PATH_REMOVED":true}})");
      if(resource == "/")
        return reply(R"({"FULL_PATH":"/","CONTENTS":{"a":)" + node_a() + "}}");
      if(resource == "/a")
      {
        readings++;
        std::this_thread::sleep_for(100ms);
        return reply(node_a());
      }
      return reply(resource.empty() || resource[0] != '/' ? "" : "{}");
    });
    server.listen(port);
    thread = std::thread{[this] { server.run(); }};
  }
  ~slow_server()
  {
    server.stop();
    thread.join();
  }
  static std::string node_a()
  {
    return R"({"FULL_PATH":"/a","CONTENTS":{"b":{"FULL_PATH":"/a/b","TYPE":"f","VALUE":[1.0],"ACCESS":3}}})";
  }
  static ossia::net::server_reply reply(const std::string& json)
  {
    ossia::net::server_reply r;
    r.type = ossia::net::server_reply::data_type::json;
    r.data = json;
    return r;
  }
  void send(const std::string& text)
  {
    std::lock_guard l{mutex};
    server.send_message(client, text);
  }
};

// Ten PATH_CHANGED of /a at once; how many times /a is read
int burst(bool queues, int ws_port, uint16_t osc_port)
{
  slow_server s{ws_port};
  auto proto = new ossia::oscquery::oscquery_mirror_protocol(
      "ws://127.0.0.1:" + std::to_string(ws_port), osc_port);
  proto->set_request_queues(queues);
  CHECK(proto->get_request_queues() == queues);
  generic_device dev{std::unique_ptr<protocol_base>(proto), "mirror"};
  REQUIRE(proto->update(dev.get_root_node()));
  REQUIRE(find_node(dev.get_root_node(), "/a/b"));
  for(int i = 0; i < 200 && !s.connected; i++)
    std::this_thread::sleep_for(10ms);
  REQUIRE(s.connected);

  for(int i = 0; i < 10; i++)
    s.send(R"({"COMMAND":"PATH_CHANGED","DATA":"/a"})");

  // Until no reading has come for 500 ms
  int last = -1;
  for(int i = 0; i < 60 && s.readings != last; i++)
  {
    last = s.readings;
    std::this_thread::sleep_for(500ms);
    proto->run_commands();
  }
  CHECK(find_node(dev.get_root_node(), "/a/b"));
  return s.readings;
}
}

TEST_CASE("test_oscquery_mirror_path_changed_burst", "[oscquery]")
{
  // Off, as before: a reading for each PATH_CHANGED
  CHECK(burst(false, 5800, 10800) == 10);
  // On: the reading under way, then once more after it
  CHECK(burst(true, 5801, 10801) <= 2);
}

TEST_CASE("test_oscquery_mirror_request_queues_reads", "[oscquery]")
{
  // The reads of a mirror with set_request_queues still get their values
  generic_device server{
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(1802, 5802), "server"};
  auto sa = server.create_child("a")->create_parameter(val_type::INT);
  sa->push_value(1);
  auto sb = server.create_child("b")->create_parameter(val_type::INT);
  sb->push_value(2);

  auto proto
      = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5802", 10802);
  proto->set_request_queues(true);
  generic_device mirror{std::unique_ptr<protocol_base>(proto), "mirror"};
  REQUIRE(proto->update(mirror.get_root_node()));

  auto& a = *find_node(mirror.get_root_node(), "/a")->get_parameter();
  auto& b = *find_node(mirror.get_root_node(), "/b")->get_parameter();
  auto& ghost_node = find_or_create_node(mirror.get_root_node(), "/ghost");
  auto& ghost = *ghost_node.create_parameter(val_type::INT);
  ghost.set_value(-1);

  sa->push_value(10);
  sb->push_value(20);
  auto fa = proto->pull_async(a);
  auto fg = proto->pull_async(ghost);
  auto fb = proto->pull_async(b);
  REQUIRE(fa.wait_for(3s) == std::future_status::ready);
  REQUIRE(fg.wait_for(3s) == std::future_status::ready);
  REQUIRE(fb.wait_for(3s) == std::future_status::ready);
  CHECK_NOTHROW(fa.get());
  CHECK_THROWS(fg.get());
  CHECK_NOTHROW(fb.get());
  CHECK(a.value() == ossia::value{10});
  CHECK(b.value() == ossia::value{20});
  CHECK(ghost.value() == ossia::value{-1});
}
