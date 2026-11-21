// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

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
// Each setup takes its own ports, in 1780-1799, 5780-5799, 10780-10799
int next_port = 0;

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

struct asio_server
{
  const int index = next_port++ % 10;
  const uint16_t osc_port = 1780 + index;
  const uint16_t ws_port = 5780 + index;
  network_context_ptr ctx = std::make_shared<network_context>();
  ossia::oscquery_asio::oscquery_server_protocol* proto
      = new ossia::oscquery_asio::oscquery_server_protocol(ctx, osc_port, ws_port);
  generic_device dev{std::unique_ptr<protocol_base>(proto), "crossing_server"};
  parameter_base& level
      = *create_node(dev.get_root_node(), "/level").create_parameter(val_type::FLOAT);

  std::atomic_bool polling{true};
  std::thread thread{[this] {
    while(polling)
    {
      ctx->context.poll();
      std::this_thread::sleep_for(1ms);
    }
  }};

  ~asio_server()
  {
    polling = false;
    thread.join();
  }

  std::string url() const { return "ws://127.0.0.1:" + std::to_string(ws_port); }
};

struct mirror
{
  ossia::oscquery::oscquery_mirror_protocol* proto;
  generic_device dev;
  parameter_base* level{};
  std::mutex mutex;
  std::vector<float> received;

  mirror(const asio_server& s, int n)
      : proto{new ossia::oscquery::oscquery_mirror_protocol(
            s.url(), uint16_t(10780 + (s.index * 2 + n) % 20))}
      , dev{std::unique_ptr<protocol_base>(proto), "crossing_mirror"}
  {
    proto->update(dev.get_root_node());
    auto node = find_node(dev.get_root_node(), "/level");
    REQUIRE(node);
    level = node->get_parameter();
    REQUIRE(level);
    // What the server sends: the mirror's own writes go through
    // push_value_quiet and call no callback
    level->add_callback([this](const ossia::value& v) {
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
  bool has(float f)
  {
    auto v = values();
    return std::find(v.begin(), v.end(), f) != v.end();
  }
  float value() const { return ossia::convert<float>(level->value()); }
};

// The server and the client write the same address in the same instant,
// 20 times: how many times they end apart
int crossed_writes(asio_server& s, mirror& m, mirror* other = nullptr)
{
  int divergences = 0;
  for(int i = 0; i < 20; i++)
  {
    std::atomic_bool go{false};
    std::thread client([&] {
      while(!go)
      {
      }
      m.level->push_value_quiet(200.f + i);
    });
    go = true;
    s.level.push_value(100.f + i);
    client.join();
    std::this_thread::sleep_for(300ms);
    const float at_server = ossia::convert<float>(s.level.value());
    if(at_server != m.value() || (other && at_server != other->value()))
    {
      divergences++;
      UNSCOPED_INFO(
          "trial " << i << ": server " << at_server << ", client " << m.value());
    }
  }
  return divergences;
}
}

TEST_CASE("test_oscquery_asio_identify_senders", "[oscquery]")
{
  for(bool identify : {false, true})
  {
    asio_server s;
    s.dev.set_echo(true);
    s.proto->set_identify_senders(identify);
    mirror a{s, 0};
    mirror b{s, 1};

    a.level->push_value_quiet(0.5f);
    REQUIRE(wait_for([&] { return b.has(0.5f); }));
    std::this_thread::sleep_for(300ms);
    INFO("identify " << identify << ": the sender received " << a.values().size());
    if(identify)
      // The sender does not get its own value back
      CHECK(a.values().empty());
    else
      // As before: it does
      CHECK(a.values() == std::vector<float>{0.5f});
  }
}

TEST_CASE("test_oscquery_asio_crossing_window_off", "[oscquery]")
{
  asio_server s;
  REQUIRE(s.proto->crossing_window() == 0ms);
  mirror m{s, 0};

  crossed_writes(s, m);
  // As before: the server never sends a client its own value
  for(float v : m.values())
    CHECK(v < 200.f);
}

TEST_CASE("test_oscquery_asio_crossed_writes_converge", "[oscquery]")
{
  asio_server s;
  s.proto->set_crossing_window(100ms);
  mirror m{s, 0};
  CHECK(crossed_writes(s, m) == 0);
}

TEST_CASE("test_oscquery_asio_crossed_writes_converge_with_echo", "[oscquery]")
{
  // The value the server forwards for the client must not hide the one it
  // sent
  asio_server s;
  s.dev.set_echo(true);
  s.proto->set_identify_senders(true);
  s.proto->set_crossing_window(100ms);
  mirror m{s, 0};
  mirror other{s, 1};
  CHECK(crossed_writes(s, m, &other) == 0);
}

TEST_CASE("test_oscquery_asio_write_that_crossed_nothing", "[oscquery]")
{
  // Out of a crossing, nothing comes back
  asio_server s;
  s.proto->set_crossing_window(100ms);
  mirror m{s, 0};

  s.level.push_value(1.f);
  REQUIRE(wait_for([&] { return m.has(1.f); }));
  // Past the window
  std::this_thread::sleep_for(300ms);
  for(int i = 0; i < 20; i++)
  {
    m.level->push_value_quiet(300.f + i);
    std::this_thread::sleep_for(20ms);
  }
  REQUIRE(wait_for([&] { return ossia::convert<float>(s.level.value()) == 319.f; }));
  std::this_thread::sleep_for(300ms);
  CHECK(m.values() == std::vector<float>{1.f});
}
