#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace ossia
{
namespace net
{
generic_device::generic_device(
    std::unique_ptr<ossia::net::protocol_base> protocol, std::string name)
    : device_base(std::move(protocol)), generic_node(std::move(name), *this)
{
  mProtocol->setDevice(*this);
  mCapabilities.change_tree = true;
}

generic_device::~generic_device()
{
  removeAddress();
  write_lock_t lock{m_mutex};
  m_children.clear();
  mProtocol.reset();
}

namespace
{
node_base* find_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  const auto& children = node.children();
  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto it = ossia::find_if(children, [&](const auto& child) {
      return child->getName() == cur;
    });
    if (it != children.end())
    {
      // There are still nodes since we found a slash
      return find_node_rec(
          **it,
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

    auto it = ossia::find_if(children, [&](const auto& child) {
      return child->getName() == address;
    });

    if (it != children.end())
    {
      return it->get();
    }
    else
    {
      return nullptr;
    }
  }
}

node_base& find_or_create_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
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
      return find_or_create_node_rec(**it,
          address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto& child = *node.createChild(cur.to_string());

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
      return *it->get();
    }
    else
    {
      // Create and return the node
      return  *node.createChild(address.to_string());
    }
  }
}

//! Note : here we modify the string_view only.
//! The original address remains unchanged.
ossia::string_view sanitize_address(ossia::string_view address)
{
  if (boost::algorithm::starts_with(address, "/"))
    address.remove_prefix(1);
  if (boost::algorithm::ends_with(address, "/"))
    address.remove_suffix(1);
  return address;
}
}

node_base* find_node(node_base& dev, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return &dev;

  // address now looks like a/b/c
  return find_node_rec(dev, address);
}

node_base&
find_or_create_node(node_base& node, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return node;

  // address now looks like a/b/c
  return find_or_create_node_rec(node, address);
}
}
}
