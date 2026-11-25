// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/config.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include "include_catch.hpp"

#include <chrono>
#include <thread>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

namespace
{
struct setup
{
  generic_device server{
      std::make_unique<ossia::oscquery::oscquery_server_protocol>(1760, 5760), "server"};
  ossia::oscquery::oscquery_mirror_protocol* proto{};
  std::unique_ptr<generic_device> mirror;

  setup()
  {
    auto a = server.create_child("a")->create_parameter(val_type::INT);
    a->push_value(1);
    auto b = server.create_child("b")->create_parameter(val_type::INT);
    b->push_value(2);

    proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5760", 10760);
    mirror = std::make_unique<generic_device>(
        std::unique_ptr<ossia::net::protocol_base>(proto), "mirror");
    proto->update(mirror->get_root_node());
  }

  parameter_base& param(std::string_view name)
  {
    auto node = find_node(mirror->get_root_node(), name);
    REQUIRE(node);
    REQUIRE(node->get_parameter());
    return *node->get_parameter();
  }

  // A parameter the mirror has and the server has not: its value read
  // fails (404)
  parameter_base& ghost()
  {
    auto& node = find_or_create_node(mirror->get_root_node(), "/ghost");
    auto p = node.get_parameter();
    if(!p)
      p = node.create_parameter(val_type::INT);
    p->set_value(-1);
    return *p;
  }
};
}

TEST_CASE("test_oscquery_value_read_after_failed_read", "[oscquery]")
{
  setup s;
  auto& a = s.param("/a");
  auto& ghost = s.ghost();

  // The read of an address the server does not have fails
  CHECK(!s.proto->pull(ghost));

  // The next read gets the value of the address it asked for
  find_node(s.server.get_root_node(), "/a")->get_parameter()->push_value(42);
  CHECK(s.proto->pull(a));
  CHECK(a.value() == ossia::value{42});
  CHECK(ghost.value() == ossia::value{-1});
}

TEST_CASE("test_oscquery_value_reads_in_flight", "[oscquery]")
{
  setup s;
  auto& a = s.param("/a");
  auto& b = s.param("/b");
  auto& ghost = s.ghost();

  find_node(s.server.get_root_node(), "/a")->get_parameter()->push_value(10);
  find_node(s.server.get_root_node(), "/b")->get_parameter()->push_value(20);

  // Several reads at once, one of them failing: each answer goes to the
  // address it was asked for
  for(int i = 0; i < 20; i++)
  {
    auto fa = s.proto->pull_async(a);
    auto fg = s.proto->pull_async(ghost);
    auto fb = s.proto->pull_async(b);
    REQUIRE(fa.wait_for(3s) == std::future_status::ready);
    REQUIRE(fg.wait_for(3s) == std::future_status::ready);
    REQUIRE(fb.wait_for(3s) == std::future_status::ready);
    CHECK_NOTHROW(fa.get());
    CHECK_THROWS(fg.get());
    CHECK_NOTHROW(fb.get());
  }
  CHECK(a.value() == ossia::value{10});
  CHECK(b.value() == ossia::value{20});
  CHECK(ghost.value() == ossia::value{-1});
}
