// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/value/value_conversion.hpp>

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
