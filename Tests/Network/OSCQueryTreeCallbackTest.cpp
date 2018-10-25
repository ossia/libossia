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

static std::vector<std::string> created_params;
static std::vector<std::string> removed_params;
static std::vector<std::string> created_nodes;
static std::vector<std::string> removed_nodes;

static std::vector<std::string> add_params;
static std::vector<std::string> rm_params;
static std::vector<std::string> add_nodes;
static std::vector<std::string> rm_nodes;
static int count{0};

#define LOOP_DELAY 500ms

void reset()
{
  created_nodes.clear();
  removed_nodes.clear();
  created_params.clear();
  removed_params.clear();
  count=0;
}

void check(opp::oscquery_mirror& client)
{
  reset();
  while(count++<10 || (
          created_nodes.size()  != add_nodes.size()
       && removed_nodes.size()  != rm_nodes.size()
       && created_params.size() != add_params.size()
       && removed_params.size() != rm_params.size()))
  {
    std::cout << "up" << std::endl;
    client.update();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  CHECK(created_nodes.size() == add_nodes.size());
  CHECK(removed_nodes.size() == rm_nodes.size());
  CHECK(created_params.size() == add_params.size());
  CHECK(removed_params.size() == rm_params.size());

  for(auto& n : add_nodes)
    CHECK(client.get_root_node().find_children(n).size() == 1);

  for(auto& n : add_params)
  {
    auto children = client.get_root_node().find_children(n);
    CHECK(children.size() == 1);
    if(!children.empty())
      CHECK(children[0].has_parameter());
  }

  for(auto& n : rm_nodes)
    CHECK(client.get_root_node().find_children(n).size() == 0);

  for(auto& n : rm_params)
  {
    auto children = client.get_root_node().find_children(n);
    if(!children.empty())
      CHECK(!children[0].has_parameter());
  }

  add_params.clear();
  rm_params.clear();
  add_nodes.clear();
  rm_nodes.clear();
}

TEST_CASE ("test_oscquery_simple_node_creation_cb", "test_oscquery_simple_node_creation_cb")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};

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
  }, &removed_nodes);

  while(!client.is_connected())
  {
    std::cerr << "damned ! les connexions ne font pas !" << std::endl;
    client.reconnect();
    std::this_thread::sleep_for(100ms);
  }

  std::cout << "step 1" << std::endl;

  serv.create_child("foo")->create_parameter(ossia::val_type::BOOL);

  add_nodes.push_back("foo");
  add_params.push_back("foo");

  check(client);

  std::cout << "step 2" << std::endl;

  auto node = serv.create_child("bar");
  add_nodes.push_back("bar");

  node->create_parameter(ossia::val_type::STRING);
  add_params.push_back("bar");

  auto nested = node->create_child("nested");
  add_nodes.push_back("bar/nested");

  nested->create_parameter(ossia::val_type::INT);
  add_params.push_back("bar/nested");

  check(client);

  std::cout << "step 3" << std::endl;
  CHECK(serv.remove_child("bar"));

  rm_nodes.push_back("bar");
  rm_params.push_back("bar");
  rm_nodes.push_back("bar/nested");
  rm_params.push_back("bar/nested");

  check(client);
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
  }, &removed_nodes);

  while(!client.is_connected())
  {
    std::cerr << "damned ! les connexions ne font pas !" << std::endl;
    client.reconnect();
    std::this_thread::sleep_for(LOOP_DELAY);
  }

  auto& root = serv.get_root_node();

  std::cout << "step 1" << std::endl;

  auto truc = root.create_child("truc");
  add_nodes.push_back("truc");
  auto muche = root.create_child("muche");
  add_nodes.push_back("muche");

  check(client);

  std::cout << "step 2" << std::endl;

  truc->create_parameter(val_type::INT);
  add_params.push_back("truc");
  muche->create_parameter(val_type::INT);
  add_params.push_back("muche");
  auto foo = muche->create_child("foo");
  add_nodes.push_back("muche/foo");

  check(client);

  std::cout << "step 3" << std::endl;

  REQUIRE(root.remove_child("truc"));
  rm_nodes.push_back("truc");

  muche->remove_parameter();
  rm_params.push_back("muche");

  foo->create_parameter(val_type::BOOL);
  add_params.push_back("muche/foo");

  check(client);

  root.remove_child("truc");
  root.remove_child("muche");
  rm_nodes.push_back("truc");
  rm_nodes.push_back("muche");

  std::cout << "step 4" << std::endl;
  check(client);
}
