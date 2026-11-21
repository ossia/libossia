// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "include_catch.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/network/base/node_functions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include <atomic>
#include <cstdio>
#include <future>
#include <thread>

// A client that walks the tree of a mirror on its own thread, and applies the
// changes the server sends there with run_commands() (as ossia-max and
// ossia-pd do), asks for the namespace read again by update_async() to be
// applied there too: by default it is applied on the network thread that
// receives it, which clears the tree and builds it again while the other
// thread walks it. Run this test under AddressSanitizer or ThreadSanitizer.
TEST_CASE("test_oscquery_namespace_in_run_commands", "[oscquery]")
{
  constexpr int tracks = 64;
  constexpr int params = 32;
  ossia::net::generic_device server{
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(1240, 10240),
      "server"};
  for(int i = 0; i < tracks; i++)
    for(int j = 0; j < params; j++)
      ossia::net::create_node(
          server.get_root_node(),
          "/track." + std::to_string(i) + "/param." + std::to_string(j))
          .create_parameter(ossia::val_type::FLOAT);

  auto proto = std::make_unique<ossia::oscquery::oscquery_mirror_protocol>(
      "ws://127.0.0.1:10240", 10241);
  auto* mirror = proto.get();
  ossia::net::generic_device client{std::move(proto), "client"};
  auto& root = client.get_root_node();

  // Read once before another thread runs the commands
  REQUIRE(mirror->update(root));
  mirror->set_namespace_in_run_commands(true);

  std::atomic_bool stop{false};
  std::size_t names = 0;
  std::thread owner([&] {
    while(!stop)
    {
      mirror->run_commands();
      for(auto* n : ossia::net::list_all_children(&root, 0, false))
      {
        names += n->get_name().size();
        if(auto* p = n->get_parameter())
          (void)p->value();
      }
    }
  });

  int applied = 0;
  for(int i = 0; i < 5; i++)
  {
    auto fut = mirror->update_async(root);
    if(fut.wait_for(std::chrono::seconds(10)) == std::future_status::ready)
      applied++;
  }
  stop = true;
  owner.join();

  std::printf(
      "namespace read again %d times of 5 (%zu characters walked)\n", applied, names);
  CHECK(applied == 5);
  CHECK(
      ossia::net::list_all_children(&root, 0, false).size()
      == std::size_t(tracks + tracks * params));
}
