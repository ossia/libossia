#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/dataspace/value_with_unit.hpp>


namespace ossia
{
namespace net
{
address_base::~address_base() = default;

static void
getAddressFromNode_rec(const node_base& n, std::string& str)
{
  if (auto p = n.getParent())
  {
    getAddressFromNode_rec(*p, str);
  }
  else
  {
    // we're at the root
    str += n.getName();
    str += ':';
    return;
  }

  str += '/';
  str += n.getName();
}

static void
getOSCAddressFromNode_rec(const node_base& n, std::string& str)
{
  if (auto p = n.getParent())
  {
    getOSCAddressFromNode_rec(*p, str);
  }
  else
  {
    // we're at the root
    return;
  }

  str += '/';
  str += n.getName();
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
  std::string s;
  s.reserve(80);
  getOSCAddressFromNode_rec(n, s);
  return s;
}

std::string osc_address_string(const address_base& addr)
{
  return osc_address_string(addr.getNode());
}

std::future<void> address_base::pullValueAsync()
{
  return {};
}

void address_base::requestValue()
{
}

value address_base::cloneValue(destination_index idx) const
{
  return get_value_at_index(cloneValue(), idx);
}

std::vector<ossia::value> address_base::cloneValue(const std::vector<destination_index>& indices) const
{
  std::vector<ossia::value> t;
  t.reserve(indices.size());

  auto v = cloneValue();
  for(auto idx : indices)
  {
    t.push_back(get_value_at_index(v, idx));
  }

  return t;
}

value address_base::fetchValue()
{
  pullValue();
  return cloneValue();
}

value address_base::getDefaultValue() const { return {}; }

address_base& address_base::setDefaultValue(const value& v) { return *this; }

unit_t address_base::getUnit() const { return {}; }

address_base& address_base::setUnit(const unit_t& v) { return *this; }

value_with_unit get_value(const ossia::Destination& d)
{
  ossia::net::address_base& addr = d.value.get();

  return make_value(addr.cloneValue(d.index), addr.getUnit());
}

void push_value(const Destination& d, const value_with_unit& v)
{
  ossia::net::address_base& addr = d.value.get();
  addr.pushValue(ossia::to_value(v)); // TODO what about destination_index ??
}


optional<value> clone_value(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->cloneValue();
  return ossia::none;
}
void set_value(ossia::net::node_base& n, value v)
{
  if(auto addr = n.getAddress()) addr->setValue(std::move(v));
}

optional<val_type> get_value_type(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getValueType();
  return ossia::none;
}
void set_value_type(ossia::net::node_base& n, val_type v)
{
  if(auto addr = n.getAddress()) addr->setValueType(std::move(v));
}

optional<domain> get_domain(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getDomain();
  return ossia::none;
}
void set_domain(ossia::net::node_base& n, domain v)
{
  if(auto addr = n.getAddress()) addr->setDomain(std::move(v));
}

optional<access_mode> get_access_mode(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getAccessMode();
  return ossia::none;
}
void set_access_mode(ossia::net::node_base& n, access_mode v)
{
  if(auto addr = n.getAddress()) addr->setAccessMode(std::move(v));
}

optional<bounding_mode> get_bounding_mode(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getBoundingMode();
  return ossia::none;
}
void set_bounding_mode(ossia::net::node_base& n, bounding_mode v)
{
  if(auto addr = n.getAddress()) addr->setBoundingMode(std::move(v));
}

optional<repetition_filter> get_repetition_filter(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getRepetitionFilter();
  return ossia::none;
}
void set_repetition_filter(ossia::net::node_base& n, repetition_filter v)
{
  if(auto addr = n.getAddress()) addr->setRepetitionFilter(std::move(v));
}

optional<unit_t> get_unit(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getUnit();
  return ossia::none;
}
void set_unit(ossia::net::node_base& n, unit_t v)
{
  if(auto addr = n.getAddress()) addr->setUnit(std::move(v));
}

optional<ossia::value> get_default_value(const ossia::net::node_base& n)
{
  if(auto addr = n.getAddress()) return addr->getDefaultValue();
  return ossia::none;
}
void set_default_value(ossia::net::node_base& n, ossia::value v)
{
  if(auto addr = n.getAddress()) addr->setDefaultValue(std::move(v));
}


}
}

std::ostream& operator<<(std::ostream& s, const ossia::net::address_base& addr)
{
  return s << ossia::net::address_string_from_node(addr);
}

