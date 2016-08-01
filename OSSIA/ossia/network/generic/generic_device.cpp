#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>

namespace ossia
{
namespace net
{
generic_device::generic_device(
    std::unique_ptr<ossia::net::protocol_base> protocol, std::string name)
    : device_base(std::move(protocol)), generic_node(std::move(name), *this)
{
  mProtocol->setDevice(*this);
}

generic_device::~generic_device()
{
  mChildren.clear();
  mProtocol.reset();
}

namespace
{
generic_node* find_node_rec(
    generic_node& node,
    boost::string_ref address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto it = ossia::find_if(node.children(), [&](const auto& child) {
      return child->getName() == cur;
    });
    if (it != node.children().end())
    {
      // There are still nodes since we found a slash
      return find_node_rec(
          dynamic_cast<ossia::net::generic_node&>(**it),
          address.substr(first_slash_index + 1));
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    // One of the child may be the researched node.

    auto it = ossia::find_if(node.children(), [&](const auto& child) {
      return child->getName() == address;
    });

    if (it != node.children().end())
    {
      return dynamic_cast<ossia::net::generic_node*>(it->get());
    }
    else
    {
      return nullptr;
    }
  }
}

generic_node& find_or_create_node_rec(
    generic_node& node,
    boost::string_ref address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto it = ossia::find_if(node.children(), [&](const auto& child) {
      return child->getName() == cur;
    });
    if (it != node.children().end())
    {
      // There are still nodes since we found a slash
      return find_or_create_node_rec(
          dynamic_cast<ossia::net::generic_node&>(**it),
          address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto& child = dynamic_cast<ossia::net::generic_node&>(
          *node.createChild(cur.to_string()));

      // Recurse on it
      return find_or_create_node_rec(
          child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // One of the child may be the researched node.
    auto it = ossia::find_if(node.children(), [&](const auto& child) {
      return child->getName() == address;
    });

    if (it != node.children().end())
    {
      return dynamic_cast<ossia::net::generic_node&>(*it->get());
    }
    else
    {
      // Create and return the node
      return dynamic_cast<ossia::net::generic_node&>(
          *node.createChild(address.to_string()));
    }
  }
}

boost::string_ref sanitize_address(boost::string_ref address)
{
  if (address.starts_with('/'))
    address.remove_prefix(1);
  if (address.ends_with('/'))
    address.remove_suffix(1);
  return address;
}
}

generic_node* find_node(generic_device& dev, boost::string_ref address)
{
  address = sanitize_address(address);
  if (address.empty())
    return &dev;

  // address now looks like a/b/c
  return find_node_rec(dev, address);
}

generic_node&
find_or_create_node(generic_device& dev, boost::string_ref address)
{
  address = sanitize_address(address);
  if (address.empty())
    return dev;

  // address now looks like a/b/c
  return find_or_create_node_rec(dev, address);
}
}
}
