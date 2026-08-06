// Validation tests for the Boost.Beast network-stack review findings.
//
// These tests are designed to be run under ThreadSanitizer and
// AddressSanitizer. They exercise the exact pattern that the review
// flagged as a data race / use-after-free:
//
//  * the OSCQuery server / websocket server run their io_context on a
//    dedicated thread (an async_read is permanently in flight), while
//  * the application thread (here: the test thread) calls send_*() which,
//    in the buggy version, performed a *synchronous* beast write on the
//    foreign thread — concurrent access to a beast websocket::stream,
//    which is documented as not thread-safe.
//
// After the fix (sends marshalled onto the connection strand via an async
// write queue) these run clean under tsan/asan.

#include <ossia/detail/config.hpp>

#include <ossia/network/context.hpp>

#include "include_catch.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

#if defined(OSSIA_PROTOCOL_OSCQUERY)
#include <ossia/network/sockets/websocket_client_beast.hpp>
#include <ossia/network/sockets/websocket_server_beast.hpp>

#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

namespace
{
void spin_until(std::function<bool()> cond, int max_ms = 4000)
{
  using namespace std::chrono;
  auto deadline = steady_clock::now() + milliseconds(max_ms);
  while(!cond() && steady_clock::now() < deadline)
    std::this_thread::sleep_for(milliseconds(5));
}
}

// Direct reproduction: the producer thread calls server.send_*() while the
// server's io thread has an async_read pending on the same connection.
TEST_CASE(
    "ws_beast_concurrent_send_read", "ws_beast_concurrent_send_read")
{
  using namespace ossia::net;
  constexpr uint16_t port = 28765;

  auto server_ctx = std::make_shared<network_context>();
  websocket_server_beast server{server_ctx};

  std::mutex hmtx;
  ws_connection_handle hdl;
  std::atomic_bool have_hdl{false};

  server.set_open_handler([&](ws_connection_handle h) {
    std::lock_guard l{hmtx};
    hdl = h;
    have_hdl = true;
  });
  server.set_close_handler([&](ws_connection_handle) { have_hdl = false; });
  server.set_message_handler(
      [](const ws_connection_handle&, ws_opcode, const std::string&) {
    return server_reply{};
  });

  server.listen(port);
  std::thread server_thread([&] { server.run(); });

  auto client_ctx = std::make_shared<network_context>();
  std::atomic_int received{0};
  websocket_client_beast client{
      client_ctx->context,
      [&](const ws_connection_handle&, ws_opcode, std::string&) { ++received; }};
  client.connect("ws://127.0.0.1:" + std::to_string(port));
  std::thread client_thread([&] { client_ctx->context.run(); });

  spin_until([&] { return have_hdl.load() && client.connected(); });
  REQUIRE(have_hdl.load());
  REQUIRE(client.connected());

  // Hammer sends from this (foreign) thread while the server io thread is
  // reading on the same connection. In the buggy version this is a data
  // race on the beast stream that tsan reports.
  const std::string payload(64, 'x');
  for(int i = 0; i < 3000; ++i)
  {
    ws_connection_handle h;
    {
      std::lock_guard l{hmtx};
      h = hdl;
    }
    server.send_binary_message(h, payload);
  }

  // The client must actually have received some of them (proves the writes
  // really happened, not just enqueued-and-dropped).
  spin_until([&] { return received.load() > 0; });
  REQUIRE(received.load() > 0);

  client.stop();
  client_ctx->context.stop();
  if(client_thread.joinable())
    client_thread.join();

  server.stop();
  server_ctx->context.stop();
  if(server_thread.joinable())
    server_thread.join();
}

// Teardown reproduction: destroy the server while a connection's async_read
// is in flight on the io thread. Exercises the close()/teardown path that the
// review flagged (the macOS libc++ SIGBUS during async-handler teardown).
TEST_CASE("ws_beast_teardown_under_load", "ws_beast_teardown_under_load")
{
  using namespace ossia::net;
  constexpr uint16_t port = 28766;

  for(int round = 0; round < 5; ++round)
  {
    auto server_ctx = std::make_shared<network_context>();
    auto server = std::make_unique<websocket_server_beast>(server_ctx);

    std::atomic_bool opened{false};
    server->set_open_handler([&](ws_connection_handle) { opened = true; });
    server->set_close_handler([&](ws_connection_handle) {});
    server->set_message_handler(
        [](const ws_connection_handle&, ws_opcode, const std::string&) {
      return server_reply{};
    });
    server->listen(port);
    std::thread server_thread([&] { server->run(); });

    auto client_ctx = std::make_shared<network_context>();
    websocket_client_beast client{
        client_ctx->context,
        [](const ws_connection_handle&, ws_opcode, std::string&) {}};
    client.connect("ws://127.0.0.1:" + std::to_string(port));
    std::thread client_thread([&] { client_ctx->context.run(); });

    spin_until([&] { return opened.load() && client.connected(); });

    // Tear everything down while reads are in flight.
    client.stop();
    client_ctx->context.stop();
    if(client_thread.joinable())
      client_thread.join();

    server->stop();
    server.reset(); // destroy server while io thread may still hold connections
    server_ctx->context.stop();
    if(server_thread.joinable())
      server_thread.join();
  }
  REQUIRE(true);
}

// High-level reproduction through the actual OSCQuery protocol: push values
// from the producer thread while the server services the mirror on its own
// io thread.
TEST_CASE("oscquery_concurrent_push", "oscquery_concurrent_push")
{
  using namespace ossia;
  constexpr uint16_t osc_port = 28767;
  constexpr uint16_t ws_port = 28768;

  net::generic_device server_dev{
      std::make_unique<oscquery::oscquery_server_protocol>(osc_port, ws_port),
      "srv"};

  auto root = server_dev.create_child("val");
  auto param = root->create_parameter(ossia::val_type::INT);
  param->push_value(0);

  net::generic_device mirror_dev{
      std::make_unique<oscquery::oscquery_mirror_protocol>(
          "ws://127.0.0.1:" + std::to_string(ws_port)),
      "mirror"};

  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  // Make the mirror observe the parameter so the server actually sends on
  // value changes (driving server-side writes from the producer thread).
  if(auto n = net::find_node(mirror_dev, "/val"))
    if(auto p = n->get_parameter())
      mirror_dev.get_protocol().observe(*p, true);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Phase 1: non-critical parameter — server pushes go over the OSC-over-UDP
  // path (client.sender). This exercises the m_clientsMutex-guarded sender
  // setup/use against the producer thread.
  param->set_critical(false);
  for(int i = 0; i < 3000; ++i)
    param->push_value(i);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Phase 2: critical parameter — server pushes are routed over the (beast)
  // WebSocket transport instead, exercising the websocket send path the review
  // flagged.
  param->set_critical(true);
  for(int i = 0; i < 3000; ++i)
    param->push_value(i);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  REQUIRE(true);
}
#endif
