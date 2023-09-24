// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "TestUtils.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/context.hpp>
#include <ossia/network/oscquery/detail/json_parser.hpp>
#include <ossia/network/oscquery/detail/json_writer.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

#include "include_catch.hpp"

#include <iostream>

using namespace ossia;
using namespace ossia::net;

TEST_CASE("test_oscquery_rename_node", "test_oscquery_rename_node")
{
  // check if node renaming is reflected in client and value is correctly updated

  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  TestDeviceRef dev{serv};
  (void)dev;

  dev.float_addr->push_value(-1234.56789f);
  auto node = dev.device.create_child("node");
  auto child = node->create_child("param");
  auto param = child->create_parameter(ossia::val_type::FLOAT);
  param->push_value(9876.5432f);

  // WS client
  auto ws_proto
      = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
  std::unique_ptr<generic_device> ws_clt{
      new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"}};

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
  {
    auto n = find_node(ws_clt->get_root_node(), "/node/param");
    REQUIRE(n);
    auto p = n->get_parameter();
    REQUIRE(p);
    REQUIRE(p->get_value_type() == val_type::FLOAT);
    auto v = p->value().get<float>();
    REQUIRE(v == 9876.5432f);
  }

  dev.float_addr->get_node().set_name("new_name");
  dev.float_addr->push_value(0.123456f);

  node->set_name("new_node_name");
  param->push_value(1234.56789f);

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
  {
    auto n = find_node(ws_clt->get_root_node(), "/new_node_name/param");
    REQUIRE(n);
    auto p = n->get_parameter();
    REQUIRE(p);
    REQUIRE(p->get_value_type() == val_type::FLOAT);
    auto v = p->value().get<float>();
    REQUIRE(v == 1234.56789f);
    p->push_value(12.3456f);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  auto v = dev.float_addr->value().get<float>();
  REQUIRE(v == 65.4321f);
  v = param->value().get<float>();
  REQUIRE(v == 12.3456f);
}

TEST_CASE("test_oscquery_rename_node_callback", "test_oscquery_rename_node_callback")
{
  // check if callback are still after renaming a node
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};

  ossia::net::parameter_base* param
      = serv.create_child("float")->create_parameter(val_type::FLOAT);
  auto node = serv.create_child("node");
  auto child = node->create_child("param");
  auto nested_param = child->create_parameter(val_type::FLOAT);

  float cb_float{};
  float client_cb_float{};
  param->add_callback([&](const value& v) { cb_float = v.get<float>(); });

  float nested_cb_float{};
  float client_nested_cb_float{};
  nested_param->add_callback([&](const value& v) { nested_cb_float = v.get<float>(); });

  // WS client
  auto ws_proto
      = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 10001);
  std::unique_ptr<generic_device> ws_clt{
      new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"}};

  ws_proto->update(ws_clt->get_root_node());

  // Wait a little bit for the server to send the namespace
  // and to network thread to process it
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  {
    auto n = find_node(ws_clt->get_root_node(), "/float");
    REQUIRE(n);
    auto p = n->get_parameter();
    REQUIRE(p);
    p->add_callback([&](const value& v) { client_cb_float = v.get<float>(); });
  }
  {
    auto n = find_node(ws_clt->get_root_node(), "/node/param");
    REQUIRE(n);
    auto p = n->get_parameter();
    REQUIRE(p);
    p->add_callback([&](const value& v) { client_nested_cb_float = v.get<float>(); });
  }

  param->push_value(-1234.56789f);
  nested_param->push_value(9876.5432f);

  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  REQUIRE(cb_float == -1234.56789f);
  REQUIRE(client_cb_float == -1234.56789f);
  REQUIRE(nested_cb_float == 9876.5432f);
  REQUIRE(client_nested_cb_float == 9876.5432f);

  param->get_node().set_name("new_name");
  param->push_value(0.123456f);

  node->set_name("new_node_name");
  nested_param->push_value(1234.56789f);

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
  {
    auto n = find_node(ws_clt->get_root_node(), "/new_node_name/param");
    REQUIRE(n);
    auto p = n->get_parameter();
    REQUIRE(p);
    REQUIRE(p->get_value_type() == val_type::FLOAT);
    auto v = p->value().get<float>();
    REQUIRE(v == 1234.56789f);
    p->push_value(12.3456f);
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  REQUIRE(cb_float == 65.4321f);
  REQUIRE(nested_cb_float == 12.3456f);
}
