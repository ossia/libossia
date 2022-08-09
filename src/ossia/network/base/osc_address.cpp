#include <ossia/network/base/osc_address.hpp>

namespace ossia::net
{

static void getAddressFromNode_rec(const node_base& n, std::string& str)
{
  if(auto p = n.get_parent())
  {
    getAddressFromNode_rec(*p, str);
  }
  else
  {
    // we're at the root
    str += n.get_name();
    str += ':';
    return;
  }

  str += '/';
  str += n.get_name();
}

static void getOSCAddressFromNode_rec(const node_base& n, std::string& str)
{
  if(auto p = n.get_parent())
  {
    getOSCAddressFromNode_rec(*p, str);
  }
  else
  {
    // we're at the root
    return;
  }

  str += '/';
  str += n.get_name();
}

static void getOSCAddressFromNodeWithDevice_rec(const node_base& n, std::string& str)
{
  if(auto p = n.get_parent())
  {
    getOSCAddressFromNode_rec(*p, str);
  }

  str += '/';
  str += n.get_name();
}

std::string address_string_from_node(const ossia::net::node_base& node)
{
  std::string s;
  s.reserve(80);
  getAddressFromNode_rec(node, s);
  if(s.back() == ':') // case of only device.
    s += '/';
  return s;
}

std::string address_string_from_node(const ossia::net::parameter_base& addr)
{
  return address_string_from_node(addr.get_node());
}

std::string relative_address_string_from_nodes(
    const ossia::net::node_base& node, const ossia::net::node_base& parent)
{
  std::string addr = ossia::net::address_string_from_node(node);
  // TODO how to deal with multiple parents ?
  std::string parent_addr = ossia::net::address_string_from_node(parent);
  if(parent_addr.back() != '/')
    parent_addr += "/";

  if(addr.find(parent_addr) != std::string::npos)
  {
    return addr.substr(parent_addr.size());
  }
  else
  {
    return node.get_name();
  }
}

std::string osc_parameter_string(const node_base& n)
{
  if(n.get_parent())
  {
    std::string s;
    s.reserve(80);
    getOSCAddressFromNode_rec(n, s);

    return s;
  }
  else
  {
    return "/";
  }
}

std::string osc_parameter_string_with_device(const node_base& n)
{
  if(n.get_parent())
  {
    std::string s;
    s.reserve(80);
    getOSCAddressFromNodeWithDevice_rec(n, s);

    return s;
  }
  else
  {
    return "/";
  }
}

std::string osc_parameter_string(const parameter_base& addr)
{
  return osc_parameter_string(addr.get_node());
}

std::string osc_parameter_string_with_device(const parameter_base& addr)
{
  return osc_parameter_string_with_device(addr.get_node());
}

}
