// A mirror and the PATH_CHANGED of a server: it reads the structure of the
// address again, as the proposal says
#include "TestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

#include <atomic>
#include <mutex>
#include <thread>

using namespace ossia;
using namespace ossia::net;

namespace
{
// A server of the test that serves a tree it is given, as JSON, and can send
// a text message to its client
struct fake_server
{
  std::mutex mutex;
  std::string root, a;
  bool has_a{true};
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
        [this](auto, auto, const std::string& resource) -> ossia::net::server_reply {
      std::lock_guard l{mutex};
      if(resource.find("HOST_INFO") != std::string::npos)
        return reply(
            R"({"NAME":"fake","EXTENSIONS":{"TYPE":true,"VALUE":true,"PATH_CHANGED":true,"PATH_ADDED":true,"PATH_REMOVED":true}})");
      if(resource == "/")
        return reply(root);
      if(resource == "/a")
      {
        if(!has_a)
          throw ossia::node_not_found_error{"/a"};
        return reply(a);
      }
      // A WebSocket command of the mirror (LISTEN...), or a value read
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

const char* node_b = R"({"FULL_PATH":"/a/b","TYPE":"f","VALUE":[1.0],"ACCESS":3})";
const char* node_c = R"({"FULL_PATH":"/a/c","TYPE":"i","VALUE":[2],"ACCESS":3})";

std::string node_a(const char* child_name, const char* child)
{
  return std::string{R"({"FULL_PATH":"/a","CONTENTS":{")"} + child_name + "\":" + child
         + "}}";
}
std::string tree(const std::string& a)
{
  return std::string{R"({"FULL_PATH":"/","CONTENTS":{"a":)"} + a + "}}";
}

template <typename F>
bool wait_for(ossia::oscquery::oscquery_mirror_protocol& m, F&& f)
{
  for(int i = 0; i < 200; i++)
  {
    m.run_commands();
    if(f())
      return true;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return false;
}
}

TEST_CASE("test_oscquery_mirror_path_changed", "test_oscquery_mirror_path_changed")
{
  fake_server s{5750};
  {
    std::lock_guard l{s.mutex};
    s.a = node_a("b", node_b);
    s.root = tree(s.a);
  }

  auto proto
      = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5750", 11750);
  generic_device dev{std::unique_ptr<protocol_base>(proto), "mirror"};
  REQUIRE(proto->update(dev.get_root_node()));
  REQUIRE(find_node(dev.get_root_node(), "/a/b"));
  for(int i = 0; i < 200 && !s.connected; i++)
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  REQUIRE(s.connected);

  // The structure of /a changed: the mirror reads it again
  {
    std::lock_guard l{s.mutex};
    s.a = node_a("c", node_c);
    s.root = tree(s.a);
  }
  s.send(R"({"COMMAND":"PATH_CHANGED","DATA":"/a"})");
  REQUIRE(wait_for(
      *proto, [&] { return find_node(dev.get_root_node(), "/a/c") != nullptr; }));

  // /a is gone: the reading gives a 404, the node goes as with PATH_REMOVED
  {
    std::lock_guard l{s.mutex};
    s.has_a = false;
  }
  s.send(R"({"COMMAND":"PATH_CHANGED","DATA":"/a"})");
  REQUIRE(wait_for(*proto, [&] {
    auto a = find_node(dev.get_root_node(), "/a");
    return a == nullptr || ossia::net::get_zombie(*a);
  }));
}
