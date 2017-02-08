/*!
 * \file publication.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <iostream>
#include <memory>
#include <functional>

#include <ossia/ossia.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>

using namespace ossia;
using namespace std;

void printValueCallback(const value& v);
void printValue(const value& v);

int main()
{
  auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
  ossia::net::local_protocol& local_proto = *local_proto_ptr;
  // declare this program "B" as Local device
  ossia::net::generic_device device{std::move(local_proto_ptr), "B"};

  /* publish each feature of program "B" as address into a tree */

  {
    // Create a node
    auto& node = ossia::net::find_or_create_node(device, "/test/my_impulse");

    // Addresses allow nodes to have values.
    auto address = node.createAddress(val_type::IMPULSE);

    // Called whenver the value is changed
    address->add_callback(printValueCallback);

    // Send a message. Impulse is a message without any content.
    address->pushValue(impulse{});
  }

  {
    auto& node = ossia::net::find_or_create_node(device, "/test/my_bool");
    auto address = node.createAddress(val_type::BOOL);
    address->add_callback(printValueCallback);
    address->pushValue(true);
  }
  {
    auto& node = ossia::net::find_or_create_node(device, "/test/my_float");
    auto address = node.createAddress(val_type::FLOAT);
    address->add_callback(printValueCallback);
    address->pushValue(1234.);
  }
  {
    auto& node = ossia::net::find_or_create_node(device, "/test/my_int");
    auto address = node.createAddress(val_type::INT);

    // Set some metadata
    net::set_access_mode(node, access_mode::GET);
    net::set_bounding_mode(node, bounding_mode::FOLD);
    net::set_domain(node, ossia::net::make_domain(2, 14));
    net::set_description(node, "an integral value");

    address->add_callback(printValueCallback);
    address->pushValue(5678);
  }

  {
    auto& node = ossia::net::find_or_create_node(device, "/test/my_char");
    auto address = node.createAddress(val_type::CHAR);
    address->add_callback(printValueCallback);
    address->pushValue('c');
  }

  {
    auto& node = ossia::net::find_or_create_node(device, "/test/my_string");
    auto address = node.createAddress(val_type::STRING);
    address->add_callback(printValueCallback);
    address->pushValue("hello world"s);
  }

  {
    // tuple is a std::vector<value>
    auto& node = ossia::net::find_or_create_node(device, "/test/my_tuple");
    auto address = node.createAddress(val_type::TUPLE);
    address->add_callback(printValueCallback);

    using tuple = std::vector<ossia::value>;
    address->pushValue(tuple{"foo"s, 1234, tuple{"bar"s, 4.5}});

    // Domain of the tuple
    ossia::net::set_domain(node,
      ossia::net::domain_base<tuple>(
                             tuple{0, 1}, // Min values
                             tuple{3, 5}, // Max values
                             boost::container::flat_set<tuple>{
                               tuple{123, 345}, tuple{12345, 234} // Allowed values
                             } ));
  }

  {
    // fixed-length arrays
    auto& node = ossia::net::find_or_create_node(device, "/test/my_vec3f");
    auto address = node.createAddress(val_type::VEC3F);
    address->add_callback(printValueCallback);
    address->pushValue(ossia::make_vec(0., 1., 2.));
  }
  {
    auto& node = ossia::net::find_or_create_node(device, "/units/vec2");
    auto address = node.createAddress(val_type::VEC2F);
    ossia::net::set_unit(node, ossia::cartesian_2d_u{});
    address->add_callback(printValueCallback);
    address->pushValue(ossia::make_vec(5., 6.));
  }

  // declare a distant program as a Minuit device
  local_proto.exposeTo(std::make_unique<oscquery::oscquery_server_protocol>(1234, 5678));

  while (true)
    ;
}

void printValueCallback(const value& v)
{
  cerr << "Callback: " << value_to_pretty_string(v) << "\n";
}

