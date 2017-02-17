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
#include <ossia/network/base/debug.hpp>
#include <iostream>
#include <memory>
#include <functional>
using namespace ossia;
using namespace ossia::net;
using namespace std;

void explore(const node_base& node);

int main()
{
  // This program is named "A" and mirrors a remote device named "B".
  ossia::net::generic_device device{
    std::make_unique<ossia::net::minuit_protocol>("A", "127.0.0.1", 6666, 9999),
        "B"};

  // explore the tree of B
  device.getProtocol().update(device);

  // display tree in console
  explore(device.getRootNode());
}

void explore(const ossia::net::node_base& node)
{
  for (const auto& child : node.children())
  {
    if (auto addr = child->getAddress())
    {
      // attach to callback to display value update
      addr->add_callback([=] (const value& v) {
        std::cerr << "Callback: " << osc_address_string(*addr)
                  << " : " <<  value_to_pretty_string(v) << std::endl;
      });

      // update the value
      addr->pullValue();
    }

    fmt::MemoryWriter w;
    w << *child;
    std::cerr << w.str() << "\n";

    explore(*child);
  }
}
