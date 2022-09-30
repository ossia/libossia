// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*!
 * \file publication.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <ossia/detail/config.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/detail/flat_set.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <iostream>
#include <memory>
#include <functional>

using namespace ossia;
using namespace std;

void printValueCallback(const value& v);
void printValue(const value& v);

int main()
{
  using namespace ossia::net;

  auto local_proto_ptr = std::make_unique<multiplex_protocol>();
  multiplex_protocol& local_proto = *local_proto_ptr;
  // declare this program "B" as Local device
  generic_device device{std::move(local_proto_ptr), "B"};

  auto onAddNode = [&] (std::string parent, parameter_data dat) {
    auto& p_node = ossia::net::find_or_create_node(device, parent);
    auto cld = p_node.create_child(dat.name);
    cld->create_parameter(ossia::val_type::INT);
  };
  device.on_add_node_requested.connect(&onAddNode);

  auto onRemoveNode = [&] (std::string parent, std::string node) {
    auto p_node = ossia::net::find_node(device, parent);
    if(p_node)
      p_node->remove_child(node);
  };
  device.on_remove_node_requested.connect(&onRemoveNode);

  /* publish each feature of program "B" as address into a tree */

  {
    // Create a node
    auto& node = find_or_create_node(device, "/test/my_impulse");

    // Addresses allow nodes to have values.
    auto address = node.create_parameter(val_type::IMPULSE);

    // Called whenever the value is changed
    address->add_callback(printValueCallback);

    // Send a message. Impulse is a message without any content.
    address->push_value(impulse{});
  }

  {
    auto& node = find_or_create_node(device, "/test/my_bool");
    auto address = node.create_parameter(val_type::BOOL);
    address->add_callback(printValueCallback);
    address->push_value(true);
  }
  {
    auto& node = find_or_create_node(device, "/test/my_float");
    auto address = node.create_parameter(val_type::FLOAT);
    address->add_callback(printValueCallback);
    address->push_value(0.5);

    std::thread t{[=] {
        while(true) {
          address->push_value(double(rand()) / RAND_MAX);
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }}};
    t.detach();
  }
  {
    auto& node = find_or_create_node(device, "/test/my_int");
    auto address = node.create_parameter(val_type::INT);

    // Set some metadata
    node.set(access_mode_attribute{}, access_mode::GET);
    node.set(bounding_mode_attribute{}, bounding_mode::FOLD);
    node.set(domain_attribute{}, make_domain(2, 14));
    node.set(description_attribute{}, "an integral value");
    node.set(default_value_attribute{}, 10);
    node.set(tags_attribute{}, net::tags{"foo", "bar"});
    node.set(refresh_rate_attribute{}, net::refresh_rate{5});
    node.set(value_step_size_attribute{}, net::value_step_size{2});
    node.set(priority_attribute{}, net::priority{100});

    address->add_callback(printValueCallback);
    address->push_value(4);
  }

  {
    auto& node = find_or_create_node(device, "/test/my_char");
    auto address = node.create_parameter(val_type::CHAR);
    address->add_callback(printValueCallback);
    address->push_value('c');
  }

  {
    auto& node = find_or_create_node(device, "/test/my_string");
    auto address = node.create_parameter(val_type::STRING);
    node.set(critical_attribute{}, true);
    node.set(domain_attribute{}, ossia::make_domain(std::vector<std::string>{"foo"s, "bar"s, "hello world"s}));
    address->add_callback(printValueCallback);
    address->push_value("hello world"s);
  }

  {
    // tuple is a std::vector<value>
    auto& node = find_or_create_node(device, "/test/my_list_values");
    auto address = node.create_parameter(val_type::LIST);
    address->add_callback(printValueCallback);

    using tuple = std::vector<ossia::value>;
    address->push_value(tuple{"foo"s, 1234, tuple{"bar"s, 4.5}});

    // Domain of the tuple
    node.set(domain_attribute{},
      vector_domain(
                             {}, // Min values
                             {}, // Max values
                             std::vector<ossia::flat_set<ossia::value>>{
                               {123, 345}, {12345, 234} // Allowed values
                             } ));
  }

  {
    // tuple is a std::vector<value>
    auto& node = find_or_create_node(device, "/test/my_list_minmax");
    auto address = node.create_parameter(val_type::LIST);
    address->add_callback(printValueCallback);

    using tuple = std::vector<ossia::value>;
    address->push_value(tuple{"foo"s, 1234, tuple{"bar"s, 4.5}});

    // Domain of the tuple
    node.set(domain_attribute{},
      vector_domain(tuple{0, 1}, // Min values
                    tuple{3, 5}, // Max values
                    {} ));
  }

  {
    // fixed-length arrays
    auto& node = find_or_create_node(device, "/test/my_vec3f");
    auto address = node.create_parameter(val_type::VEC3F);
    address->set_domain(
          ossia::make_domain(
            ossia::make_vec(0, -1, -2),
            ossia::make_vec(1, 1, 2)
            )
         );
    address->add_callback(printValueCallback);
    address->push_value(ossia::make_vec(0., 1., 2.));
  }
  {
    auto& node = find_or_create_node(device, "/units/vec2");
    auto address = node.create_parameter(val_type::VEC2F);
    node.set(unit_attribute{}, ossia::cartesian_2d_u{});
    address->add_callback(printValueCallback);
    address->push_value(ossia::make_vec(5., 6.));
  }
  {
    auto& node = find_or_create_node(device, "/units/float");
    auto address = node.create_parameter(val_type::INT);
    node.set(unit_attribute{}, ossia::decibel_u{});
    address->add_callback(printValueCallback);
  }

  {
    auto& node = find_or_create_node(device, "/units/rgba8");
    auto address = node.create_parameter(val_type::VEC4F);
    node.set(unit_attribute{}, ossia::rgba8_u{});
    address->add_callback(printValueCallback);
  }
  // declare a distant program as an OSCQuery device
  auto oscq = std::make_unique<oscquery::oscquery_server_protocol>(1234, 5678);
  ossia::net::network_logger n;
  n.inbound_logger = spdlog::stdout_logger_mt("console");
  n.outbound_logger = n.inbound_logger;
  oscq->set_logger(n);
  local_proto.expose_to(std::move(oscq));

  while (true)
    ;
}

void printValueCallback(const value& v)
{
  cerr << "Callback: " << value_to_pretty_string(v) << "\n";
}

