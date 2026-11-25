// refresh_followed_values: many parameters nothing follows read as one tree
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
#include <vector>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

namespace
{
// A server of the test with /g/p0 ... /g/p39, all of value `value`, that
// counts the readings of /g and the value reads under it
struct counting_server
{
  std::atomic_int value{0};
  std::atomic_int tree_reads{0};
  std::atomic_int value_reads{0};
  ossia::net::websocket_server server;
  std::thread thread;

  explicit counting_server(int port)
  {
    server.set_open_handler([](auto) { });
    server.set_close_handler([](auto) { });
    server.set_message_handler(
        [this](auto, auto, const std::string& resource) -> ossia::net::server_reply {
      if(resource.find("HOST_INFO") != std::string::npos)
        return reply(R"({"NAME":"counting","EXTENSIONS":{"TYPE":true,"VALUE":true}})");
      if(resource == "/")
        return reply(R"({"FULL_PATH":"/","CONTENTS":{"g":)" + node_g() + "}}");
      if(resource == "/g")
      {
        tree_reads++;
        return reply(node_g());
      }
      if(resource.rfind("/g/", 0) == 0)
      {
        // As ossia's server answers a value read: VALUE without an array
        value_reads++;
        return reply(R"({"VALUE":)" + std::to_string(value.load()) + "}");
      }
      return reply(resource.empty() || resource[0] != '/' ? "" : "{}");
    });
    server.listen(port);
    thread = std::thread{[this] { server.run(); }};
  }
  ~counting_server()
  {
    server.stop();
    thread.join();
  }
  std::string node_g()
  {
    std::string g = R"({"FULL_PATH":"/g","CONTENTS":{)";
    for(int i = 0; i < 40; i++)
      g += (i ? "," : "") + std::string{"\"p"} + std::to_string(i)
           + R"(":{"FULL_PATH":"/g/p)" + std::to_string(i) + R"(","TYPE":"i","VALUE":[)"
           + std::to_string(value.load()) + R"(],"ACCESS":3})";
    return g + "}}";
  }
  static ossia::net::server_reply reply(const std::string& json)
  {
    ossia::net::server_reply r;
    r.type = ossia::net::server_reply::data_type::json;
    r.data = json;
    return r;
  }
};

std::vector<parameter_base*> params(generic_device& dev, int n)
{
  std::vector<parameter_base*> res;
  for(int i = 0; i < n; i++)
  {
    auto node = find_node(dev.get_root_node(), "/g/p" + std::to_string(i));
    REQUIRE(node);
    res.push_back(node->get_parameter());
  }
  return res;
}

template <typename F>
bool wait_for(ossia::oscquery::oscquery_mirror_protocol& m, F&& f)
{
  for(int i = 0; i < 300; i++)
  {
    m.run_commands();
    if(f())
      return true;
    std::this_thread::sleep_for(10ms);
  }
  return false;
}
}

TEST_CASE("test_oscquery_mirror_bulk_refresh", "[oscquery]")
{
  for(bool queues : {false, true})
  {
    const int port = queues ? 5807 : 5806;
    counting_server s{port};
    auto proto = new ossia::oscquery::oscquery_mirror_protocol(
        "ws://127.0.0.1:" + std::to_string(port), queues ? 10807 : 10806);
    proto->set_request_queues(queues);
    generic_device dev{std::unique_ptr<protocol_base>(proto), "mirror"};
    REQUIRE(proto->update(dev.get_root_node()));

    // Past the window of the namespace reading
    std::this_thread::sleep_for(1100ms);

    // Forty parameters: one reading of /g, no value read
    s.value = 7;
    auto all = params(dev, 40);
    ossia::oscquery::refresh_followed_values(all);
    REQUIRE(wait_for(*proto, [&] {
      for(auto* p : all)
        if(p->value() != ossia::value{7})
          return false;
      return true;
    }));
    CHECK(s.tree_reads == 1);
    CHECK(s.value_reads == 0);

    // Right after it, a few of them: brought by that reading, not read
    ossia::oscquery::refresh_followed_values(params(dev, 4));
    std::this_thread::sleep_for(200ms);
    CHECK(s.value_reads == 0);

    // An explicit read asks the server, whatever the tree reading
    proto->set_read_unfollowed(true);
    s.value = 8;
    ossia::oscquery::read_followed_value(*all[0]);
    REQUIRE(wait_for(*proto, [&] { return all[0]->value() == ossia::value{8}; }));
    CHECK(s.value_reads == 1);

    // Past the window, a few: each is read
    std::this_thread::sleep_for(1100ms);
    ossia::oscquery::refresh_followed_values(params(dev, 4));
    REQUIRE(wait_for(*proto, [&] { return s.value_reads == 5; }));
    CHECK(s.tree_reads == 1);
  }
}
