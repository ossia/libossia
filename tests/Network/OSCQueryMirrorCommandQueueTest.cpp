// The commands of a mirror come from the WebSocket thread (PATH_ADDED) and
// the HTTP thread (the readings of a PATH_CHANGED) at once: none is lost.
// Under ThreadSanitizer ossia's spsc_queue is a multi-producer queue, which
// hides the race: the test is meant for a build without it.
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

#include <atomic>
#include <chrono>
#include <mutex>
#include <string>
#include <thread>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

namespace
{
struct fake_server
{
  std::mutex mutex;
  ossia::net::websocket_server::connection_handler client;
  std::atomic_bool connected{false};
  ossia::net::websocket_server server;
  std::thread thread;

  explicit fake_server(int port)
  {
    server.set_open_handler([this](auto hdl) {
      std::lock_guard l{mutex};
      client = hdl;
      connected = true;
    });
    server.set_close_handler([](auto) { });
    server.set_message_handler(
        [](auto, auto, const std::string& resource) -> ossia::net::server_reply {
      if(resource.find("HOST_INFO") != std::string::npos)
        return reply(
            R"({"NAME":"fake","EXTENSIONS":{"TYPE":true,"VALUE":true,"PATH_CHANGED":true,"PATH_ADDED":true}})");
      if(resource == "/")
        return reply(R"({"FULL_PATH":"/","CONTENTS":{"a":)" + node_a() + "}}");
      if(resource == "/a")
        return reply(node_a());
      return reply(resource.empty() || resource[0] != '/' ? "" : "{}");
    });
    server.listen(port);
    thread = std::thread{[this] { server.run(); }};
  }
  ~fake_server()
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
}

TEST_CASE("test_oscquery_mirror_commands_from_two_threads", "[oscquery]")
{
  fake_server s{5808};
  auto proto
      = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5808", 10808);
  generic_device dev{std::unique_ptr<protocol_base>(proto), "mirror"};
  REQUIRE(proto->update(dev.get_root_node()));
  for(int i = 0; i < 200 && !s.connected; i++)
    std::this_thread::sleep_for(10ms);
  REQUIRE(s.connected);

  // The HTTP thread puts the readings of /a while the WebSocket thread puts
  // the nodes added
  constexpr int n = 3000;
  for(int i = 0; i < n; i++)
  {
    s.send(R"({"COMMAND":"PATH_ADDED","DATA":"/n)" + std::to_string(i) + "\"}");
    if(i % 3 == 0)
      s.send(R"({"COMMAND":"PATH_CHANGED","DATA":"/a"})");
  }

  auto all_there = [&] {
    for(int i = 0; i < n; i++)
      if(!find_node(dev.get_root_node(), "/n" + std::to_string(i)))
        return false;
    return find_node(dev.get_root_node(), "/a/b") != nullptr;
  };
  bool ok = false;
  for(int i = 0; i < 3000 && !ok; i++)
  {
    proto->run_commands();
    ok = all_there();
    if(!ok)
      std::this_thread::sleep_for(10ms);
  }
  CHECK(ok);
}
