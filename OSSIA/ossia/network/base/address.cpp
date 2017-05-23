#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/value_with_unit.hpp>


namespace ossia
{
namespace net
{
address_base::~address_base()
{

}

static void
getAddressFromNode_rec(const node_base& n, std::string& str)
{
  if (auto p = n.get_parent())
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

static void
getOSCAddressFromNode_rec(const node_base& n, std::string& str)
{
  if (auto p = n.get_parent())
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

static void
getOSCAddressFromNodeWithDevice_rec(const node_base& n, std::string& str)
{
  if (auto p = n.get_parent())
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

std::string address_string_from_node(const ossia::net::address_base& addr)
{
  return address_string_from_node(addr.getNode());
}

std::string osc_address_string(const node_base& n)
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

std::string osc_address_string_with_device(const node_base& n)
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

std::string osc_address_string(const address_base& addr)
{
  return osc_address_string(addr.getNode());
}

std::string osc_address_string_with_device(const address_base& addr)
{
  return osc_address_string_with_device(addr.getNode());
}

std::future<void> address_base::pull_value_async()
{
  return {};
}

void address_base::request_value()
{
}

value address_base::value(destination_index idx) const
{
  return get_value_at_index(value(), idx);
}

std::vector<ossia::value> address_base::value(const std::vector<destination_index>& indices) const
{
  std::vector<ossia::value> t;
  t.reserve(indices.size());

  auto v = value();
  for(auto idx : indices)
  {
    t.push_back(get_value_at_index(v, idx));
  }

  return t;
}

value address_base::fetch_value()
{
  pull_value();
  return value();
}

unit_t address_base::get_unit() const { return {}; }

address_base& address_base::set_unit(const unit_t& v) { return *this; }

bool address_base::get_muted() const { return {}; }

address_base& address_base::set_muted(bool v) { return *this; }

bool address_base::get_critical() const { return {}; }

address_base& address_base::set_critical(bool v) { return *this; }

value_with_unit get_value(const ossia::Destination& d)
{
  ossia::net::address_base& addr = d.value.get();

  return make_value(addr.value(d.index), addr.get_unit());
}

void push_value(const Destination& d, const value_with_unit& v)
{
  ossia::net::address_base& addr = d.value.get();
  addr.push_value(ossia::to_value(v)); // TODO what about destination_index ??
}

std::ostream& operator<<(std::ostream& s, const ossia::net::address_base& addr)
{
  return s << ossia::net::address_string_from_node(addr);
}
}

}
