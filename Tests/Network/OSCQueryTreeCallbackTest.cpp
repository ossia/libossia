// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <iostream>
#include <future>

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/base/parameter_data.hpp>
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

static std::vector<std::pair<std::string, std::string>> renamed_nodes;

static std::vector<std::string> add_params;
static std::vector<std::string> rm_params;
static std::vector<std::string> add_nodes;
static std::vector<std::string> rm_nodes;
static std::vector<std::pair<std::string, std::string>> rn_nodes;

#define LOOP_DELAY 500ms

void reset()
{
  created_nodes.clear();
  removed_nodes.clear();
  created_params.clear();
  removed_params.clear();
}

void check(opp::oscquery_mirror& client)
{
  int i = 0;
  int count = 0;
  reset();
  while(count++<10)
  {
    std::cout << "up" << std::endl;
    client.update();
    std::this_thread::sleep_for(LOOP_DELAY);

    if( created_nodes.size()  == add_nodes.size()
        && removed_nodes.size()  == rm_nodes.size()
        && created_params.size() == add_params.size()
        && removed_params.size() == rm_params.size() )
      break;
  }

  std::cout << "check add_nodes" << std::endl;
  CHECK(created_nodes.size() == add_nodes.size());
  i=0;
  for(auto& n : add_nodes)
  {
    std::cout << "\t" << n << std::endl;
    if (i<created_nodes.size() && i<add_nodes.size())
    {
      std::cout << "'" << created_nodes[i] << "' vs '" << add_nodes[i] << std::endl;
      CHECK(created_nodes[i] == add_nodes[i]);
    }
    CHECK(client.get_root_node().find_children(n).size() == 1);
    i++;
  }

  std::cout << "check rm_nodes" << std::endl;
  CHECK(removed_nodes.size() == rm_nodes.size());
  i=0;
  for(auto& n : rm_nodes)
  {
    std::cout << "\t" << n << std::endl;
    if (i<removed_nodes.size() && i<rm_nodes.size())
    {
      CHECK(removed_nodes[i] == rm_nodes[i]);
    }
    CHECK(client.get_root_node().find_children(n).size() == 0);
    i++;
  }

  std::cout << "check add_params" << std::endl;
  CHECK(created_params.size() == add_params.size());
  i=0;
  for(auto& n : add_params)
  {
    std::cout << "\t" << n << std::endl;
    if(i<created_params.size() && i<add_params.size())
    {
      CHECK(created_params[i] == add_params[i]);
    }
    auto children = client.get_root_node().find_children(n);
    if(!children.empty())
      CHECK(children[0].has_parameter());
    i++;
  }

  std::cout << "check rm_params" << std::endl;
  CHECK(removed_params.size() == rm_params.size());
  i=0;
  for(auto& n : rm_params)
  {
    std::cout << "\t" << n << std::endl;
    if(i<removed_params.size() && i<rm_params.size())
    {
      CHECK(removed_params[i] == rm_params[i]);
    }
    auto children = client.get_root_node().find_children(n);
    if(!children.empty())
      CHECK(!children[0].has_parameter());
    i++;
  }

  add_params.clear();
  rm_params.clear();
  add_nodes.clear();
  rm_nodes.clear();
}

class server_callback_handler
{
  public:
    server_callback_handler(generic_device& dev) : m_dev(dev)
    {
      m_dev.on_rename_node_requested.connect<&server_callback_handler::rename_cb>(*this);
      m_dev.on_remove_node_requested.connect<&server_callback_handler::remove_cb>(*this);
      m_dev.on_add_node_requested.connect<&server_callback_handler::add_cb>(*this);
    }

    void rename_cb(std::string node_str, std::string new_name)
    {
      std::cout << "rename_cb" << std::endl;
      auto nodes = ossia::net::find_nodes(m_dev.get_root_node(),node_str);
      for(auto n : nodes)
      {
        n->set_name(new_name);
      }
    }

    void remove_cb(std::string parent, std::string name)
    {
      std::cout << "remove_cb" << std::endl;
      auto nodes = ossia::net::find_nodes(m_dev.get_root_node(), parent);
      for(auto node : nodes)
      {
        if(!node->remove_child(name))
        {
          std::cerr << "can't remove node: " << name << std::endl;
        }
      }
    }

    void add_cb(std::string parent, const parameter_data& param)
    {
      std::cout << "add_cb, parent: " << parent << "\tparam name: " << param.name << std::endl;
      auto nodes = ossia::net::find_nodes(m_dev.get_root_node(), parent);

      for(auto n : nodes)
      {
        auto& node = static_cast<ossia::net::generic_node&>
                     (ossia::net::find_or_create_node(m_dev.get_root_node(), param.name));
        node.set_parameter(std::make_unique<generic_parameter>(param,node));
      }
    }
  private:
    generic_device& m_dev;

};

TEST_CASE ("test_oscquery_simple_node_creation_cb", "test_oscquery_simple_node_creation_cb")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};

  server_callback_handler sch(serv);

  int step = 0;

  opp::oscquery_mirror client("B");
  client.refresh();
  client.set_zombie_on_remove(false);
  client.set_parameter_created_callback(
        [](void* context, const opp::node& n)
  {
    const std::string addr = n.get_address();
    std::cout << "create parameter : " << addr << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(addr);
  }, &created_params);

  client.set_parameter_removed_callback(
        [](void* context, const opp::node& n)
  {
    const std::string addr = n.get_address();
    std::cout << "remove parameter : " << addr << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(addr);
  }, &removed_params);

  client.set_node_created_callback(
        [](void* context, const opp::node& n)
  {
    const std::string addr = n.get_address();
    std::cout << "create node : " << addr << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(addr);
  }, &created_nodes);

  client.set_node_removed_callback(
        [](void* context, const opp::node& n)
  {
    const std::string addr = n.get_address();
    std::cout << "remove node : " << addr << std::endl;
    std::vector<std::string>* vec = static_cast<std::vector<std::string>*>(context);
    vec->push_back(addr);
  }, &removed_nodes);

  client.set_node_renamed_callback(
        [](void* context, const opp::node& n, std::string s)
  {
    const std::string addr = n.get_address();
    auto vec = static_cast<std::vector<std::pair<std::string, std::string>>*>(context);
    vec->push_back({s, addr});
    auto pair = vec->back();
    std::cout << "renamed callback: " << pair.first << " becomes " << pair.second << std::endl;

  }, &renamed_nodes);

  while(!client.is_connected())
  {
    std::cerr << "damned ! les connexions ne font pas !" << std::endl;
    client.reconnect();
    std::this_thread::sleep_for(100ms);
  }

/*

  std::cout << "step " << step++ << std::endl;
  std::cout << "-> Check node with parameter creation callback" << std::endl;

  serv.create_child("foo")->create_parameter(ossia::val_type::BOOL);

  add_nodes.push_back("/foo");
  add_params.push_back("/foo");

  check(client);

  std::cout << "step " << step++ << std::endl;
  std::cout << "-> Check node without parameter creation callback" << std::endl;

  auto node = serv.create_child("bar");
  add_nodes.push_back("/bar");

  check(client);

  std::cout << "step " << step++ << std::endl;

  auto first = serv.create_child("first");
  add_nodes.push_back("/first");
  auto second = serv.create_child("second");
  add_nodes.push_back("/second");
  second->create_parameter(ossia::val_type::FLOAT);
  add_params.push_back("/second");
  first->create_parameter(ossia::val_type::FLOAT);
  add_params.push_back("/first");

  check(client);

  std::cout << "step " << step++ << std::endl;

  std::cout << "-> Check the callback when a nested node is created" << std::endl;
  auto nested = node->create_child("nested");
  add_nodes.push_back("/bar/nested");

  check(client);

  std::cout << "step " << step++ << std::endl;
  std::cout << "-> Check the callback when a parameter is added to a nested node" << std::endl;

  nested->create_parameter(ossia::val_type::INT);
  add_params.push_back("/bar/nested");

  check(client);

  std::cout << "step " << step++ << std::endl;
  std::cout << "-> Check the callback when a parameter is added to a root node" << std::endl;

  node->create_parameter(ossia::val_type::STRING);
  add_params.push_back("/bar");

  check(client);

  std::cout << "step " << step++ << std::endl;
  std::cout << "-> Check the callback when a node with child and parameter is delete" << std::endl;

  CHECK(serv.remove_child("bar"));

  rm_nodes.push_back("/bar/nested");
  rm_params.push_back("/bar/nested");
  rm_nodes.push_back("/bar");
  rm_params.push_back("/bar");

  check(client);

  {
    reset();
    std::cout << "-> Check tree modification request from client" << std::endl;
    std::string name = "new_node";
    client.request_add_node(client.get_root_node(), name);

    int count=0;

    // wait 10 * LOOP_DELAY for server's network thread
    // to process request and client to update
    bool flag = false;
    while(count++<10)
    {
      std::cout << "server up" << std::endl;
      serv_proto->run_commands();
      std::this_thread::sleep_for(LOOP_DELAY);

      auto nodes = ossia::net::find_nodes(serv.get_root_node(), name);

      flag = nodes.size() > 0;
      if(flag)
        break;
    }
    CHECK(flag);

    add_nodes.push_back("/" + name);
    add_params.push_back("/" + name);
    check(client);

    auto node = client.get_root_node().find_child("new_node");
    std::cout << "/new_node has a parameter ? " << node.has_parameter() << std::endl;
  }

  {
    reset();
    std::cout << "-> Check renaming request from client" << std::endl;
    std::string name = "weird_name";
    serv.create_child(name);
    add_nodes.push_back("/" + name);

    check(client);

    auto weird_node = client.get_root_node().find_child(name);
    std::string new_name = "another_cool_name";
    client.request_rename_node(weird_node, new_name);

    int count=0;
    bool flag = false;
    while(count++<10)
    {
      std::cout << "server up" << std::endl;
      serv_proto->run_commands();
      std::this_thread::sleep_for(LOOP_DELAY);

      auto nodes = ossia::net::find_nodes(serv.get_root_node(), new_name);

      flag = nodes.size() > 0;
      if(flag)
        break;
    }
    CHECK(flag);
  }

  {
    reset();
    std::cout << "-> Check node removing request from node" << std::endl;
    std::string name = "node_to_be_deleted";
    serv.create_child(name);
    add_nodes.push_back("/" + name);

    check(client);

    auto node = client.get_root_node().find_child(name);
    client.request_remove_node(node);

    int count = 0;
    bool flag = false;
    while(count++<10)
    {
      std::cout << "server up" << std::endl;
      serv_proto->run_commands();
      std::this_thread::sleep_for(LOOP_DELAY);

      auto nodes = ossia::net::find_nodes(serv.get_root_node(), name);

      flag = nodes.size() == 0;
      if(flag)
      {
        rm_nodes.push_back("/" + name);
        check(client);
        break;
      }
    }
    CHECK(flag);
  }

  {
    std::cout << "-> Check for renaming node callback" << std::endl;
    auto cool_node = serv.create_child("cool");
    add_nodes.push_back("/cool");
    cool_node->create_parameter(ossia::val_type::FLOAT);
    add_params.push_back("/cool");

    check(client);

    cool_node->set_name("super");
    rn_nodes.push_back({"/cool","/super"});

    check(client);
  }
*/
  {
    std::cout << "-> Check parameter creation callback for each type" << std::endl;

    serv.create_child("my_bool")->create_parameter(ossia::val_type::BOOL);
    serv.create_child("my_char")->create_parameter(ossia::val_type::CHAR);
    serv.create_child("my_float")->create_parameter(ossia::val_type::FLOAT);
    serv.create_child("my_impulse")->create_parameter(ossia::val_type::IMPULSE);
    serv.create_child("m_int")->create_parameter(ossia::val_type::INT);
    serv.create_child("my_list")->create_parameter(ossia::val_type::LIST);
    serv.create_child("my_none"); //->create_parameter(ossia::val_type::NONE);
    serv.create_child("my_string")->create_parameter(ossia::val_type::STRING);
    serv.create_child("my_vec2f")->create_parameter(ossia::val_type::VEC2F);
    serv.create_child("my_vec3f")->create_parameter(ossia::val_type::VEC3F);
    serv.create_child("my_vec4f")->create_parameter(ossia::val_type::VEC4F);

    add_nodes.push_back("/my_bool");
    add_nodes.push_back("/my_char");
    add_nodes.push_back("/my_float");
    add_nodes.push_back("/my_impulse");
    add_nodes.push_back("/m_int");
    add_nodes.push_back("/my_list");
    add_nodes.push_back("/my_none");
    add_nodes.push_back("/my_string");
    add_nodes.push_back("/my_vec2f");
    add_nodes.push_back("/my_vec3f");
    add_nodes.push_back("/my_vec4f");

    add_params.push_back("/my_bool");
    add_params.push_back("/my_char");
    add_params.push_back("/my_float");
    add_params.push_back("/my_impulse");
    add_params.push_back("/m_int");
    add_params.push_back("/my_list");
    // add_params.push_back("/my_none");
    add_params.push_back("/my_string");
    add_params.push_back("/my_vec2f");
    add_params.push_back("/my_vec3f");
    add_params.push_back("/my_vec4f");

    check(client);
  }
}

void node_created_cb(ossia::net::node_base& n)
{
  auto addr = ossia::net::osc_parameter_string(n);
  std::cout << "create node : " << addr << std::endl;
  created_nodes.push_back(addr);
}

void node_removing_cb(ossia::net::node_base& n)
{
  auto addr = ossia::net::osc_parameter_string(n);
  std::cout << "removing node : " << addr << std::endl;
  removed_nodes.push_back(addr);
}

void param_created_cb(const ossia::net::parameter_base& p)
{
  auto& n = p.get_node();
  auto addr = ossia::net::osc_parameter_string(n);
  std::cout << "create param: " << addr << std::endl;
  created_params.push_back(addr);
}

void param_removing_cb(const ossia::net::parameter_base& p)
{
  auto& n = p.get_node();
  auto addr = ossia::net::osc_parameter_string(n);
  std::cout << "removing param : " << addr << std::endl;
  removed_params.push_back(addr);
}

void check2(ossia::net::generic_device& client)
{
  int count=0;
  int i=0;
  reset();
  while(count++<10)
  {
    std::cout << "up" << std::endl;
    static_cast<ossia::oscquery::oscquery_mirror_protocol&>(
        client.get_protocol())
        .run_commands();
    std::this_thread::sleep_for(LOOP_DELAY);


    if( created_nodes.size()  == add_nodes.size()
        && removed_nodes.size()  == rm_nodes.size()
        && created_params.size() == add_params.size()
        && removed_params.size() == rm_params.size() )
      break;
  }

  std::cout << "check add_nodes" << std::endl;
  CHECK(created_nodes.size() == add_nodes.size());
  i=0;
  for(auto& n : add_nodes)
  {
    std::cout << "\t" << n << std::endl;
    CHECK(created_nodes[i] == add_nodes[i]);
    auto nodes = ossia::net::find_nodes(client.get_root_node(), n);
    CHECK(nodes.size() == 1);
    i++;
  }

  std::cout << "check add_params" << std::endl;
  CHECK(created_params.size() == add_params.size());
  i=0;
  for(auto& n : add_params)
  {
    std::cout << "\t" << n << std::endl;
    CHECK(created_params[i] == add_params[i]);
    auto children = ossia::net::find_nodes(client.get_root_node(), n);
    CHECK(children.size() == 1);
    if(!children.empty())
      CHECK(children[0]->get_parameter() != nullptr);
    i++;
  }

  std::cout << "check rm_nodes" << std::endl;
  CHECK(removed_nodes.size() == rm_nodes.size());
  i=0;
  for(auto& n : rm_nodes)
  {
    std::cout << "\t" << n << std::endl;
    CHECK(removed_nodes[i] == rm_nodes[i]);
    auto nodes = ossia::net::find_nodes(client.get_root_node(), n);
    CHECK(nodes.size() == 0);
    i++;
  }

  std::cout << "check rm_params" << std::endl;
  CHECK(removed_params.size() == rm_params.size());
  i=0;
  for(auto& n : rm_params)
  {
    std::cout << "\t" << n << std::endl;
    CHECK(removed_params[i] == rm_params[i]);
    auto children = ossia::net::find_nodes(client.get_root_node(), n);
    if(!children.empty())
      CHECK(children[0]->get_parameter() == nullptr);
    i++;
  }

  add_params.clear();
  rm_params.clear();
  add_nodes.clear();
  rm_nodes.clear();
}

TEST_CASE ("test_fast_fast_simple_node_creation_cb", "test_fast_fast_simple_node_creation_cb")
{

  std::cout << "\n==========="
            << "\n|FAST-FAST|"
            << "\n===========\n";
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};

  auto ws_proto = new ossia::oscquery::oscquery_mirror_protocol("ws://127.0.0.1:5678", 1234);
  ws_proto->set_zombie_on_remove(false);
  auto ws_clt = new generic_device{std::unique_ptr<ossia::net::protocol_base>(ws_proto), "B"};

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ws_proto->update(ws_clt->get_root_node());

  reset();

  ws_clt->on_node_created.connect<&node_created_cb>();
  ws_clt->on_node_removing.connect<&node_removing_cb>();
  ws_clt->on_parameter_created.connect<&param_created_cb>();
  ws_clt->on_parameter_removing.connect<&param_removing_cb>();

  std::cout << "step 1" << std::endl;

  serv.create_child("foo")->create_parameter(ossia::val_type::BOOL);

  add_nodes.push_back("/foo");
  add_params.push_back("/foo");

  check2(*ws_clt);

  std::cout << "step 2" << std::endl;

  auto node = serv.create_child("bar");
  add_nodes.push_back("/bar");

  node->create_parameter(ossia::val_type::STRING);
  add_params.push_back("/bar");

  auto nested = node->create_child("nested");
  add_nodes.push_back("/bar/nested");

  nested->create_parameter(ossia::val_type::INT);
  add_params.push_back("/bar/nested");

  check2(*ws_clt);

  std::cout << "step 3" << std::endl;
  CHECK(serv.remove_child("bar"));

  rm_nodes.push_back("/bar/nested");
  rm_params.push_back("/bar/nested");
  rm_nodes.push_back("/bar");
  rm_params.push_back("/bar");

  check2(*ws_clt);
}
