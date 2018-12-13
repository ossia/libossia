// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <ossia/detail/config.hpp>

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia-cpp/ossia-cpp98.hpp>

using namespace ossia;
using namespace ossia::net;
using namespace std::chrono_literals;

static std::vector<std::string> type_string = {
  "FLOAT",
  "INT",
  "VEC2F",
  "VEC3F",
  "VEC4F",
  "IMPULSE",
  "BOOL",
  "STRING",
  "LIST",
  "CHAR",
  "NONE"
};

static ossia::value expected_value;
static std::atomic_bool busy_flag=false;

void wait()
{
  int count = 0;
  while(busy_flag && count++<500)
  {
    std::this_thread::sleep_for(10ms);
  }
}
TEST_CASE ("test_oscquery_simple_node_creation_cb", "test_oscquery_simple_node_creation_cb")
{
  auto serv_proto = new ossia::oscquery::oscquery_server_protocol{1234, 5678};
  generic_device serv{std::unique_ptr<ossia::net::protocol_base>(serv_proto), "A"};
  opp::oscquery_mirror client("B");

  client.refresh();
  client.set_zombie_on_remove(false);

  while(!client.is_connected())
  {
    std::cerr << "damned ! les connexions ne font pas !" << std::endl;
    client.reconnect();
    std::this_thread::sleep_for(100ms);
  }

  {
    std::cout << "-> Create a parameter for each type" << std::endl;
    auto param_cb = [&](const ossia::value& v) {
      int i = std::clamp(static_cast<int>(v.get_type()),0,static_cast<int>(type_string.size()-1));
      std::cout << "receive value type "
                << type_string[i]
                << " value : " << v << std::endl;
      CHECK( v == expected_value );
      busy_flag = false;

    };
    {
      auto param = serv.create_child("my_bool")->create_parameter(ossia::val_type::BOOL);
      param->add_callback(param_cb);
    }

    {
      auto param = serv.create_child("my_char")   ->create_parameter(ossia::val_type::CHAR);
      param->add_callback(param_cb);
    }
    {
      auto param = serv.create_child("my_float")  ->create_parameter(ossia::val_type::FLOAT);
      param->add_callback(param_cb);
    }
    {
      auto param = serv.create_child("my_impulse")->create_parameter(ossia::val_type::IMPULSE);
      param->add_callback(param_cb);
    }
    {
      auto param = serv.create_child("my_int")     ->create_parameter(ossia::val_type::INT);
      param->add_callback(param_cb);
    }
    {
      auto param = serv.create_child("my_list")   ->create_parameter(ossia::val_type::LIST);
      param->add_callback(param_cb);
      std::vector<ossia::value> data;
      for(int i=0; i<10; i++)
      {
        data.push_back(i);
      }
      ossia::value val(data);
      busy_flag = true;
      expected_value = val;
      param->push_value(val);
      wait();
    }
    {
      auto param = serv.create_child("my_string") ->create_parameter(ossia::val_type::STRING);
      param->add_callback(param_cb);
    }
    {
      auto param = serv.create_child("my_vec2f")  ->create_parameter(ossia::val_type::VEC2F);
      param->add_callback(param_cb);
    }
    {
      auto param = serv.create_child("my_vec3f")  ->create_parameter(ossia::val_type::VEC3F);
      param->add_callback(param_cb);
    }
    {
      auto param = serv.create_child("my_vec4f")  ->create_parameter(ossia::val_type::VEC4F);
      param->add_callback(param_cb);
    }
  }

  // wait for network to propagate the new nodes
  std::this_thread::sleep_for(100ms);
  // update client to apply node creation
  client.update();

  {
    auto node = client.get_root_node().find_child("my_bool");
    CHECK(node.get_value().is_bool());
    opp::value val{true};
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_char");
    opp::value val{'r'};
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_float");
    CHECK(node.get_value().is_float());
    opp::value val{3.4};
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_impulse");
    CHECK(node.get_value().is_impulse());
    opp::value val{opp::value::impulse{}};
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_int");
    CHECK(node.get_value().is_int());
    opp::value val{33};
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_list");
    CHECK(node.get_value().is_list());
    std::vector<opp::value> data;
    for (int i = 0; i<10; i++)
    {
      data.push_back(i*5);
    }
    opp::value val(data);
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    // Check list of list
    auto node = client.get_root_node().find_child("my_list");
    CHECK(node.get_value().is_list());
    std::vector<opp::value> data;
    for (int i = 0; i<3; i++)
    {
      std::vector<opp::value> val{i,i+10};
      data.push_back(val);
    }
    opp::value val(data);
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_string");
    CHECK(node.get_value().is_string());
    opp::value val("C'est Noël !");
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_vec2f");
    CHECK(node.get_value().is_vec2f());
    opp::value val{opp::value::vec2f{1,2}};
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_vec3f");
    CHECK(node.get_value().is_vec3f());
    opp::value val{opp::value::vec3f{1,2,3}};
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  {
    auto node = client.get_root_node().find_child("my_vec4f");
    CHECK(node.get_value().is_vec4f());
    opp::value val{opp::value::vec4f{1,2,3,4}};
    busy_flag = true;
    expected_value = *val.get_raw_value_pointer();
    node.set_value(val);
    wait();
  }

  std::cout << "done" << std::endl;
}
