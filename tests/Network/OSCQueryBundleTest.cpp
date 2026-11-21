// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// OSC bundles sent by an OSCQuery client (the mirror) and by an OSCQuery
// server, as read on the wire.

#include "OSCQueryBundleTestUtils.hpp"
#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/base/bundle.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>

// The client's push_bundle sends the values of many parameters in OSC
// bundles: over UDP, no datagram above 1400 bytes.
TEST_CASE("oscquery_mirror_push_bundle", "oscquery_mirror_push_bundle")
{
  const int n = 3000;
  sink_server server{n};
  auto client = std::make_shared<ossia::net::generic_device>(
      std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
          server.url(), free_port(false)),
      "server");
  REQUIRE(client->get_protocol().update(client->get_root_node()));

  std::vector<const ossia::net::parameter_base*> params;
  for(int i = 0; i < n; i++)
  {
    auto* node
        = ossia::net::find_node(client->get_root_node(), "/load/p" + std::to_string(i));
    REQUIRE(node);
    auto* p = node->get_parameter();
    p->set_value(ossia::value(float(i)));
    params.push_back(p);
  }
  server.sink.clear();

  const bool sent = client->get_protocol().push_bundle(params);
  const bool all = server.sink.wait_messages(n, 3000);
  std::lock_guard lock{server.sink.mutex};
  INFO(
      "datagrams " << server.sink.datagrams.size() << ", messages "
                   << server.sink.messages.size());
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
    auto* p = ossia::net::find_node(
                  server.dev->get_root_node(), "/load/p" + std::to_string(i))
                  ->get_parameter();
    p->set_value(ossia::value(float(i)));
    params.push_back(p);
  }
  client_sink.clear();
  server.dev->get_protocol().push_bundle(params);
  const bool all = client_sink.wait_messages(n, 3000);
  std::lock_guard lock{client_sink.mutex};
  INFO(
      "datagrams " << client_sink.datagrams.size() << ", messages "
                   << client_sink.messages.size());
  REQUIRE(all);
  REQUIRE(client_sink.datagrams.size() == 1);
  REQUIRE(client_sink.messages.size() == std::size_t(n));
}

// Bundles of values: each element carries its own value, so a parameter may
// come several times. The client's mirror and the OSCQuery server send them
// as bundles; the server splits them under 1400 bytes. Before, the default of
// protocol_base set each value again and pushed it alone.
TEST_CASE("oscquery_value_bundles", "oscquery_value_bundles")
{
  const int n = 3000;
  sink_server server{n};

  SECTION("client: the values of the elements, in order, in one datagram")
  {
    auto client = std::make_shared<ossia::net::generic_device>(
        std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
            server.url(), free_port(false)),
        "server");
    REQUIRE(client->get_protocol().update(client->get_root_node()));
    auto* p0
        = ossia::net::find_node(client->get_root_node(), "/load/p0")->get_parameter();
    std::vector<ossia::bundle_element> elements{{p0, 1.f}, {p0, 2.f}, {p0, 3.f}};
    server.sink.clear();
    REQUIRE(
        client->get_protocol().push_bundle(std::span<ossia::bundle_element>(elements)));
    REQUIRE(server.sink.wait_messages(3, 3000));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::lock_guard lock{server.sink.mutex};
    const std::vector<std::pair<std::string, float>> expected{
        {"/load/p0", 1.f}, {"/load/p0", 2.f}, {"/load/p0", 3.f}};
    REQUIRE(server.sink.messages == expected);
    REQUIRE(server.sink.datagrams.size() == 1);
  }

  SECTION("server: 3000 values in datagrams of at most 1400 bytes")
  {
    udp_sink client_sink{free_port(false)};
    raw_client client{server.url()};
    REQUIRE(client.ws.connected());
    client.stream_to(client_sink.socket.local_endpoint().port());
    client.listen_load(n);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::vector<ossia::bundle_element> elements;
    for(int i = 0; i < n; i++)
      elements.push_back(
          {ossia::net::find_node(
               server.dev->get_root_node(), "/load/p" + std::to_string(i))
               ->get_parameter(),
           float(i)});
    ossia::oscquery_asio::oscquery_server_protocol_base* proto{};
    for(auto& p : server.multiplex->get_protocols())
      if(auto* s
         = dynamic_cast<ossia::oscquery_asio::oscquery_server_protocol_base*>(p.get()))
        proto = s;
    REQUIRE(proto);
    ossia::net::protocol_base& base = *proto;
    REQUIRE(base.push_bundle(std::span<ossia::bundle_element>(elements)));
    const bool all = client_sink.wait_messages(n, 3000);
    std::lock_guard lock{client_sink.mutex};
    std::size_t largest = 0;
    for(auto size : client_sink.datagrams)
      largest = std::max(largest, size);
    INFO("datagrams " << client_sink.datagrams.size() << ", largest " << largest);
    REQUIRE(all);
    REQUIRE(largest <= 1400);
    REQUIRE(client_sink.datagrams.size() < std::size_t(n) / 20);
    REQUIRE(
        client_sink.messages[1234]
        == std::pair<std::string, float>{"/load/p1234", 1234.f});
  }
}
