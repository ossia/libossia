#include <ossia/network/common/complex_type.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/address.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/detail/apply.hpp>
namespace ossia
{

struct val_type_visitor {
  using ret = ossia::val_type;
  ret operator()(ossia::val_type v) const {
    return v;
  }
  ret operator()(const ossia::unit_t& v) const {
    return ossia::matching_type(v);
  }
  ret operator()(const ossia::extended_type& v) const {
    auto t = ossia::underlying_type(v);
    if(!t.empty())
    {
      return t[0];
    }
    return ossia::val_type::IMPULSE;
  }

  ret operator()() { return ossia::val_type::IMPULSE; }
};

ossia::val_type underlying_type(const complex_type& t)
{
  return ossia::apply(val_type_visitor{}, t);
}

struct setup_address_visitor {
  using ret = ossia::net::address_base*;
  ossia::net::node_base& n;
  ret operator()(ossia::val_type v) const {
    return n.create_address(v);
  }
  ret operator()(const ossia::unit_t& v) const {
    auto addr = n.create_address(ossia::matching_type(v));
    addr->set_unit(v);
    return addr;
  }
  ret operator()(const ossia::extended_type& v) const {
    auto t = ossia::underlying_type(v);
    if(!t.empty())
    {
      auto addr = n.create_address(t[0]);
      ossia::net::set_extended_type(n, v);
      return addr;
    }
    return nullptr;
  }

  ret operator()() { return nullptr; }
};

net::address_base* setup_address(const complex_type& t, net::node_base& node)
{
  if(!t)
    return nullptr;

  return ossia::apply(setup_address_visitor{node}, t);
}


struct update_address_visitor {
  using ret = void;
  ossia::net::address_base& addr;
  ret operator()(ossia::val_type v) const {
    addr.set_value_type(v);
  }
  ret operator()(const ossia::unit_t& v) const {
    addr.set_unit(v);
  }
  ret operator()(const ossia::extended_type& v) const {
    auto t = ossia::underlying_type(v);
    if(!t.empty())
    {
      addr.set_value_type(t[0]);
    }
    ossia::net::set_extended_type(addr.getNode(), v);
  }

  ret operator()() { }
};

void update_address_type(const complex_type& t, net::address_base& addr)
{
  ossia::apply(update_address_visitor{addr}, t);
}

}
