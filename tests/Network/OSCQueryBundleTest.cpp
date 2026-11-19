// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// OSC bundles sent by an OSCQuery client (the mirror) and by an OSCQuery
// server, as read on the wire.

#include <ossia/detail/config.hpp>

#include <ossia/network/oscquery/oscquery_mirror.hpp>

#include "OSCQueryBundleTestUtils.hpp"
#include "include_catch.hpp"

// The client's push_bundle sends the values of many parameters in OSC
// bundles: over UDP, no datagram above 1400 bytes.
TEST_CASE("oscquery_mirror_push_bundle", "oscquery_mirror_push_bundle")
{
  const int n = 3000;
  sink_server server{n};
  auto client = std::make_shared<ossia::net::generic_device>(
      std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(server.url(), free_port(false)),
      "server");
  REQUIRE(client->get_protocol().update(client->get_root_node()));

  std::vector<const ossia::net::parameter_base*> params;
  for(int i = 0; i < n; i++)
  {
    auto* node = ossia::net::find_node(client->get_root_node(), "/load/p" + std::to_string(i));
    REQUIRE(node);
    auto* p = node->get_parameter();
    p->set_value(ossia::value(float(i)));
    params.push_back(p);
  }
  server.sink.clear();

  const bool sent = client->get_protocol().push_bundle(params);
  const bool all = server.sink.wait_messages(n, 3000);
  std::lock_guard lock{server.sink.mutex};
  INFO("datagrams " << server.sink.datagrams.size() << ", messages " << server.sink.messages.size());
  REQUIRE(sent);
  REQUIRE(all);
  REQUIRE(server.sink.messages.size() == std::size_t(n));
  for(auto size : server.sink.datagrams)
    REQUIRE(size <= 1400);
  REQUIRE(server.sink.datagrams.size() < std::size_t(n) / 20);
  REQUIRE(server.sink.messages[1234].first == "/load/p1234");
  REQUIRE(server.sink.messages[1234].second == 1234.f);
}

// An OSCQuery server under a multiplex, as ossia-max exposes a device, sends
// a bundle as one OSC bundle to each client; before, the multiplex pushed its
// values one by one.
TEST_CASE("oscquery_multiplex_push_bundle", "oscquery_multiplex_push_bundle")
{
  const int n = 40;
  sink_server server{n};
  udp_sink client_sink{free_port(false)};
  raw_client client{server.url()};
  REQUIRE(client.ws.connected());
  client.stream_to(client_sink.socket.local_endpoint().port());
  client.listen_load(n);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::vector<const ossia::net::parameter_base*> params;
  for(int i = 0; i < n; i++)
  {
    auto* p = ossia::net::find_node(server.dev->get_root_node(), "/load/p" + std::to_string(i))
                  ->get_parameter();
    p->set_value(ossia::value(float(i)));
    params.push_back(p);
  }
  client_sink.clear();
  server.dev->get_protocol().push_bundle(params);
  const bool all = client_sink.wait_messages(n, 3000);
  std::lock_guard lock{client_sink.mutex};
  INFO("datagrams " << client_sink.datagrams.size() << ", messages " << client_sink.messages.size());
  REQUIRE(all);
  REQUIRE(client_sink.datagrams.size() == 1);
  REQUIRE(client_sink.messages.size() == std::size_t(n));
}
