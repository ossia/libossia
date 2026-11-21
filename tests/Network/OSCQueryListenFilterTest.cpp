// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/context.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

#include "include_catch.hpp"
#include "OSCQueryHttpTestUtils.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <optional>
#include <thread>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

namespace
{
bool wait_for(const std::function<bool()>& f, std::chrono::milliseconds d = 2000ms)
{
  for(auto end = std::chrono::steady_clock::now() + d;
      std::chrono::steady_clock::now() < end; std::this_thread::sleep_for(10ms))
    if(f())
      return true;
  return f();
}

std::string command(const std::string& cmd, const std::string& path)
{
  return "{\"COMMAND\":\"" + cmd + "\",\"DATA\":\"" + path + "\"}";
}

// Two clients: one sent LISTEN for /alpha, the other sent nothing. Once the
// clients that get /alpha have it, and a moment after, what they were going
// to get has come.
template <typename Server>
void listen_filter(
    generic_device& dev, Server& server, int ws_port, bool filtered,
    const std::function<void(parameter_base&, parameter_base&)>& bundle = {})
{
  server.set_listen_filter(filtered);
  auto alpha = dev.create_child("alpha")->create_parameter(val_type::FLOAT);
  auto beta = dev.create_child("beta")->create_parameter(val_type::FLOAT);

  // The clients leave before the server stops: a server stopping on
  // connections that are closing is not what this test is about
  std::atomic_int gone{0};
  auto on_gone = [&](const std::string&) { gone++; };
  server.onClientDisconnected.connect(on_gone);
  struct disconnect
  {
    Server& server;
    decltype(on_gone)& f;
    ~disconnect() { server.onClientDisconnected.disconnect(f); }
  } disconnect_on_exit{server, on_gone};
  std::optional<ossia_test::raw_ws_client> listener_c, silent_c;
  listener_c.emplace(ws_port);
  silent_c.emplace(ws_port);
  auto& listener = *listener_c;
  auto& silent = *silent_c;
  REQUIRE(listener.connected());
  REQUIRE(silent.connected());
  listener.send(command("LISTEN", "/alpha"));
  std::this_thread::sleep_for(100ms);

  auto settle = [&](int alphas) {
    REQUIRE(wait_for([&] { return listener.count("/alpha") >= alphas; }));
    if(!filtered)
      REQUIRE(wait_for([&] { return silent.count("/alpha") >= alphas; }));
    std::this_thread::sleep_for(100ms);
  };

  alpha->push_value(1.f);
  beta->push_value(2.f);
  settle(1);
  if(filtered)
  {
    CHECK(listener.count("/beta") == 0);
    CHECK(silent.count("/alpha") == 0);
    CHECK(silent.count("/beta") == 0);
  }
  else
  {
    CHECK(listener.count("/beta") == 1);
    CHECK(silent.count("/beta") == 1);
  }

  if(bundle)
  {
    // A bundle: each client gets the addresses it listens to
    bundle(*alpha, *beta);
    settle(2);
    CHECK(listener.count("/beta") == (filtered ? 0 : 2));
    CHECK(silent.count("/beta") == (filtered ? 0 : 2));
  }

  // IGNORE: the listener gets /alpha no more once filtered
  listener.send(command("IGNORE", "/alpha"));
  std::this_thread::sleep_for(100ms);
  const int before = listener.count("/alpha");
  alpha->push_value(3.f);
  if(filtered)
  {
    std::this_thread::sleep_for(300ms);
    CHECK(listener.count("/alpha") == before);
  }
  else
  {
    CHECK(wait_for([&] { return listener.count("/alpha") == before + 1; }));
  }

  listener_c.reset();
  silent_c.reset();
  CHECK(wait_for([&] { return gone == 2; }, 5000ms));
}

void websocketpp_server(bool filtered)
{
  auto proto = new ossia::oscquery::oscquery_server_protocol{1772, 5772};
  generic_device dev{std::unique_ptr<protocol_base>(proto), "listen"};
  CHECK(!proto->get_listen_filter());
  listen_filter(dev, *proto, 5772, filtered);
}

void asio_server(bool filtered)
{
  auto ctx = std::make_shared<network_context>();
  auto proto = new ossia::oscquery_asio::oscquery_server_protocol{ctx, 1773, 5773};
  auto dev = std::make_unique<generic_device>(
      std::unique_ptr<protocol_base>(proto), "listen_asio");
  std::thread t{[ctx] { ctx->run(); }};
  CHECK(!proto->get_listen_filter());
  listen_filter(*dev, *proto, 5773, filtered, [&](parameter_base& a, parameter_base& b) {
    a.set_value(4.f);
    b.set_value(5.f);
    proto->push_bundle({&a, &b});
  });
  ctx->context.stop();
  t.join();
  dev.reset();
}
}

TEST_CASE("test_oscquery_listen_filter_off", "[oscquery]")
{
  // As before: every client gets every value
  websocketpp_server(false);
  asio_server(false);
}

TEST_CASE("test_oscquery_listen_filter_on", "[oscquery]")
{
  websocketpp_server(true);
  asio_server(true);
}
