/*!
 * \file exploration.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <ossia/ossia.hpp>
#include <ossia/network/common/debug.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <iostream>
#include <memory>
#include <functional>
#include <ossia/network/oscquery/detail/http_client.hpp>
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

  // Create a device that wil attach to this protocol
  ossia::net::generic_device device{std::unique_ptr<protocol_base>(protocol), "B"};

  // Explore the tree of B
  std::cerr << "update: " << device.getProtocol().update(device) << std::endl;

  // Display the tree in console
  explore(device.getRootNode());

  auto node = ossia::net::find_node(device, "/test/my_float");
  // Request to add an instance :
  protocol->requestAddNode(*node, {"layer"});

  // Again : (will become layer.1)
  protocol->requestAddNode(*node, {"layer"});

  // Wait a bit to get a reply
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // This will apply the changes to the tree.
  protocol->runCommands();

  // Wait a bit
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Display the tree with the new nodes
  explore(device.getRootNode());
}

void explore(const ossia::net::node_base& node)
{
  for (const auto& child : node.children_copy())
  {
    if (auto addr = child->getAddress())
    {
      // attach to callback to display value update
      addr->add_callback([=] (const value& v) { /*
        std::cerr << "[message] " << osc_address_string(*addr)
                  << " <- " <<  value_to_pretty_string(v) << std::endl;
                  */
      });

      // update the value
      addr->pullValue();
    }

    using namespace fmt;
    fmt::MemoryWriter w;
    w << *child;
    std::cerr << w.str() << "\n";

    explore(*child);
  }
}
