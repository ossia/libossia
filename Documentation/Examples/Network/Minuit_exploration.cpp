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
#include <ossia/network/local/local.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/minuit/minuit.hpp>
#include <ossia/network/common/debug.hpp>
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
  device.get_protocol().update(device);

  // display tree in console
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
        std::cerr << "Callback: " << osc_parameter_string(*addr)
                  << " : " <<  value_to_pretty_string(v) << std::endl;
      });

      // update the value
      addr->pull_value();
    }

    fmt::print("{}\n", *child);

    explore(*child);
  }
}
