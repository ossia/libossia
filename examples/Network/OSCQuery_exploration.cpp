// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*!
 * \file exploration.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <ossia/detail/config.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/http/http_client.hpp>
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <iostream>
#include <memory>
#include <functional>
using namespace ossia;
using namespace ossia::net;
using namespace std;

void explore(const node_base& node);
void printDomain(const domain& d);
void printValueCallback(const value& v);
int main()
{
  // Create a protocol that will connect to the given websocket address
  auto protocol = new ossia::oscquery::oscquery_mirror_protocol{"ws://127.0.0.1:5678"};
  protocol->set_logger(network_logger{ossia::logger_ptr(), ossia::logger_ptr()});

  // Create a device that will attach to this protocol
  ossia::net::generic_device device{std::unique_ptr<protocol_base>(protocol), "B"};

  // Explore the tree of B
  std::cerr << "update: " << device.get_protocol().update(device) << std::endl;

  // Display the tree in console
  explore(device.get_root_node());

  while(true)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    protocol->run_commands();
  }

  ossia::net::find_node(device, "/test/my_string")->get_parameter()->push_value("fheakoezp");


  auto node = ossia::net::find_node(device, "/test/my_float");
  // Request to add an instance :
  protocol->request_add_node(*node, parameter_data{"layer"});

  // Again : (will become layer.1)
  protocol->request_add_node(*node, parameter_data{"layer"});

  // Wait a bit to get a reply
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // This will apply the changes to the tree.
  protocol->run_commands();

  // Wait a bit
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Display the tree with the new nodes
  explore(device.get_root_node());
}

void explore(const ossia::net::node_base& node)
{
  for (const auto& child : node.children_copy())
  {
    if (auto addr = child->get_parameter())
    {
      // attach to callback to display value update
      addr->add_callback([=] (const value& v) {
        std::cerr << "[message] " << osc_parameter_string(*addr)
                  << " <- " <<  value_to_pretty_string(v) << std::endl;
      });

      // update the value
      addr->pull_value();
    }

    using namespace fmt;
    fmt::print(stderr, "{}\n", *child);

    explore(*child);
  }
}
