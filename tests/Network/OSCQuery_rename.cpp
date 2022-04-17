// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <iostream>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;

TEST_CASE ("test_oscquery_rename_node", "test_oscquery_rename_node")
{
  // Here we check that the fullpath JSON is correctly parsed
  // and that all mirror parameter value are set correctly
  // with a call to ws_proto->update(...);

  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  TestDeviceRef dev{serv}; (void) dev;

  dev.float_addr->push_value(-1234.56789f);

  // WS client
  auto ws_proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
  std::unique_ptr<generic_device> ws_clt{new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"}};

  ws_proto->update(ws_clt->get_root_node());

  // Wait a little bit for the server to send the namespace
  // and to network thread to process it
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  {
    auto n = find_node(ws_clt->get_root_node(), "/float");
    REQUIRE(n);
    auto p = n->get_parameter();
    REQUIRE(p);
    REQUIRE(p->get_value_type() == val_type::FLOAT);
    auto v = p->value().get<float>();
    REQUIRE(v == -1234.56789f);
  }

  dev.float_addr->get_node().set_name("new_name");
  dev.float_addr->push_value(0.123456f);

  ws_proto->update(ws_clt->get_root_node());
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  {
    auto n = find_node(ws_clt->get_root_node(), "/new_name");
    REQUIRE(n);
    auto p = n->get_parameter();
    REQUIRE(p);
    REQUIRE(p->get_value_type() == val_type::FLOAT);
    auto v = p->value().get<float>();
    REQUIRE(v == 0.123456f);
    p->push_value(65.4321f);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  auto v = dev.float_addr->value().get<float>();
  REQUIRE(v == 65.4321f);
}
