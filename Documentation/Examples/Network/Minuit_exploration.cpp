/*!
 * \file exploration.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include <ossia/OSSIA.hpp>

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
  device.getProtocol().update(device.getRootNode());

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
      cout << "\t";
      parent = parent->getParent();
    }

    cout << child->getName();

    address_base* addr = child->getAddress();

    if (addr)
    {
      // attach to callback to display value update
      addr->addCallback(printValueCallback);

      // update the value
      addr->pullValue();

      // display address info
      cout << " : ";
      cout << getValueAsString(addr->cloneValue());

      cout << ", AccessMode(";
      switch (addr->getAccessMode())
      {
        case access_mode::SET :
        {
          cout << "set";
          break;
        }
        case access_mode::GET :
        {
          cout << "get";
          break;
        }
        case access_mode::BI :
        {
          cout << "bi";
          break;
        }
        default:
          break;
      }

      cout << "), BoundingMode(";
      switch (addr->getBoundingMode())
      {
        case bounding_mode::FREE :
        {
          cout << "free";
          break;
        }
        case bounding_mode::CLIP :
        {
          cout << "clip";
          break;
        }
        case bounding_mode::WRAP :
        {
          cout << "wrap";
          break;
        }
        case bounding_mode::FOLD :
        {
          cout << "fold";
          break;
        }
        default:
          break;
      }

      cout << "), Domain(";
      printDomain(addr->getDomain());
      cout << ")";
    }

    cout << "\n";

    explore(*child);
  }
}

void printDomain(const domain& d)
{
  /* TODO
  printvalue(d->getMin());
  cout << ", ";
  printvalue(d->getMax());
  if (!d->getvalues().empty())
  {
    cout << ", ";
    for (const auto & v : d->getvalues())
      printvalue(v);
  }
  */
}

void printValueCallback(const value& v)
{
  cout << "Callback: " << getValueAsString(v) << "\n";
}
