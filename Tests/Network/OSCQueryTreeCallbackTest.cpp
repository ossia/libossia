// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <iostream>
#include <future>

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia-cpp/ossia-cpp98.hpp>
#include "TestUtils.hpp"

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

struct TestNestedNodes
{
  ossia::net::node_base& root;
  ossia::net::node_base* foo_node = root.create_child("foo");
  ossia::net::node_base* bar_node = root.create_child("bar");
  ossia::net::parameter_base* foo_param = foo_node->create_parameter(val_type::FLOAT);
  ossia::net::parameter_base* bar_param = bar_node->create_parameter(val_type::FLOAT);
};

/*
TEST_CASE ("test_oscquery_simple_node_creation_cb", "test_oscquery_simple_node_creation_cb")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  // TestDeviceRef dev{serv}; (void) dev;

  std::vector<std::string> created_names;
  std::vector<std::string> removed_names;

  opp::oscquery_mirror client("B");
  client.refresh();
  client.set_parameter_created_callback(
        [](void* context, const opp::node& n)
  {
    std::cout << "create parameter : " << n.get_name() << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(n.get_name());
  }, &created_names);

  client.set_parameter_removed_callback(
        [](void* context, const opp::node& n)
  {
    std::cout << "remove parameter : " << n.get_name() << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(n.get_name());
  }, &removed_names);

  while(!client.is_connected())
  {
    std::cerr << "damned ! les connexions ne font pas !" << std::endl;
    client.reconnect();
    std::this_thread::sleep_for(100ms);
  }

  find_or_create_node(serv, "/main");

  serv.create_child("foo")->create_parameter(ossia::val_type::BOOL);

  auto node = serv.create_child("bar");
  node->create_parameter(ossia::val_type::STRING);
  node->create_child("nested")->create_parameter(ossia::val_type::INT);

  std::this_thread::sleep_for(1s);
  std::cout << "update client" << std::endl;
  client.update();
  std::this_thread::sleep_for(1s);

  serv.remove_child("bar");

  std::cout << "update client" << std::endl;
  client.update();
  std::this_thread::sleep_for(1s);

  std::cout << "vector size:"
            << "\ncreated: " << created_names.size()
            << "\nremoved: " << removed_names.size()
            << std::endl;
  std::cout << "update client" << std::endl;
  client.update();
  std::this_thread::sleep_for(1s);
}
*/

std::vector<std::string> created_params;
std::vector<std::string> removed_params;
std::vector<std::string> created_nodes;
std::vector<std::string> removed_nodes;
int count{0};

#define LOOP_DELAY 500ms

void reset()
{
  created_nodes.clear();
  removed_nodes.clear();
  created_params.clear();
  removed_params.clear();
  count=0;
}

TEST_CASE ("test_oscquery_complex_node_creation_cb", "test_oscquery_complex_node_creation_cb")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};

  opp::oscquery_mirror client("B");
  client.refresh();
  client.set_parameter_created_callback(
        [](void* context, const opp::node& n)
  {
    std::cout << "create parameter : " << n.get_name() << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(n.get_name());
  }, &created_params);

  client.set_parameter_removed_callback(
        [](void* context, const opp::node& n)
  {
    std::cout << "remove parameter : " << n.get_name() << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(n.get_name());
  }, &removed_params);

  client.set_node_created_callback(
        [](void* context, const opp::node& n)
  {
    std::cout << "create node : " << n.get_name() << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(n.get_name());
  }, &created_nodes);

  client.set_node_removed_callback(
        [](void* context, const opp::node& n)
  {
    std::cout << "remove node : " << n.get_name() << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(n.get_name());
  }, &removed_params);

  while(!client.is_connected())
  {
    std::cerr << "damned ! les connexions ne font pas !" << std::endl;
    client.reconnect();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  auto& root = serv.get_root_node();

  auto truc = root.create_child("truc");
  auto muche = root.create_child("muche");

  while(created_nodes.empty() && count++<4)
  {
    std::cout << "update client" << std::endl;
    client.update();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  std::cout << "step 1" << std::endl;
  REQUIRE(created_nodes.size() == 2);
  REQUIRE(removed_nodes.empty());
  REQUIRE(created_params.empty());
  REQUIRE(removed_params.empty());

  reset();

  truc->create_parameter(val_type::INT);
  muche->create_parameter(val_type::INT);
  auto foo = muche->create_child("foo");

  while(created_params.empty() && count++<4)
  {
    std::cout << "update client" << std::endl;
    client.update();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  while(created_nodes.empty() && count++<8)
  {
    std::cout << "update client" << std::endl;
    client.update();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  std::cout << "step 2" << std::endl;
  REQUIRE(created_nodes.size() == 1);
  REQUIRE(removed_nodes.empty());
  REQUIRE(created_params.size() == 2);
  REQUIRE(removed_params.empty());

  reset();

  root.remove_child("truc");
  muche->remove_parameter();
  foo->create_parameter(val_type::BOOL);

  while(removed_params.empty() && count++<8)
  {
    std::cout << "update client" << std::endl;
    client.update();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  std::cout << "step 3" << std::endl;
  REQUIRE(created_nodes.empty());
  REQUIRE(removed_nodes.size() == 1);
  REQUIRE(created_params.size() == 1);
  REQUIRE(removed_params.size() == 2);

  reset();

  root.remove_child("truc");
  root.remove_child("muche");

  while(removed_nodes.empty())
  {
    std::cout << "update client" << std::endl;
    client.update();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  std::cout << "step 4" << std::endl;
  REQUIRE(created_nodes.empty());
  REQUIRE(removed_nodes.size() == 2);
  REQUIRE(created_params.empty());
  REQUIRE(removed_params.empty());

  reset();



  /*
  std::cout << "remove 'nested' node" << std::endl;
  root.remove_child("nested");

  std::cout << "update client" << std::endl;
  client.update();
  std::this_thread::sleep_for(LOOP_DELAY);
*/
  std::cout << "vector size:"
            << "\ncreated: " << created_params.size()
            << "\nremoved: " << removed_params.size()
            << std::endl;
  std::cout << "update client" << std::endl;
  client.update();
  std::this_thread::sleep_for(LOOP_DELAY);
}
