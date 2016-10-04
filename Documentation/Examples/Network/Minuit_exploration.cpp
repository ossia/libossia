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
    // indentation
    auto parent = child->getParent();
    while (parent != nullptr)
    {
      cerr << "\t";
      parent = parent->getParent();
    }

    cerr << child->getName();

    address_base* addr = child->getAddress();

    if (addr)
    {
      // attach to callback to display value update
      addr->add_callback(printValueCallback);

      // update the value
      addr->pullValue();

      // display address info
      cerr << " : ";
      cerr << value_to_pretty_string(addr->cloneValue());

      cerr << ", AccessMode(";
      switch (addr->getAccessMode())
      {
        case access_mode::SET :
        {
          cerr << "set";
          break;
        }
        case access_mode::GET :
        {
          cerr << "get";
          break;
        }
        case access_mode::BI :
        {
          cerr << "bi";
          break;
        }
        default:
          break;
      }

      cerr << "), BoundingMode(";
      switch (addr->getBoundingMode())
      {
        case bounding_mode::FREE :
        {
          cerr << "free";
          break;
        }
        case bounding_mode::CLIP :
        {
          cerr << "clip";
          break;
        }
        case bounding_mode::WRAP :
        {
          cerr << "wrap";
          break;
        }
        case bounding_mode::FOLD :
        {
          cerr << "fold";
          break;
        }
        default:
          break;
      }

      cerr << "), Domain(";
      printDomain(addr->getDomain());
      cerr << ")";
    }

    cerr << "\n";

    explore(*child);
  }
}

void printDomain(const domain& d)
{
  /* TODO
  printvalue(d->getMin());
  cerr << ", ";
  printvalue(d->getMax());
  if (!d->getvalues().empty())
  {
    cerr << ", ";
    for (const auto & v : d->getvalues())
      printvalue(v);
  }
  */
}

void printValueCallback(const value& v)
{
  cerr << "Callback: " << value_to_pretty_string(v) << "\n";
}
