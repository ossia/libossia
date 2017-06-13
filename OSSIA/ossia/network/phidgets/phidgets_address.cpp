#include "phidgets_address.hpp"
#include "phidgets_device.hpp"
#include "phidgets_node.hpp"
#include "phidgets_protocol.hpp"

namespace ossia
{

phidget_address::phidget_address(
    ppp::phidget_ptr p,
    phidget_protocol& proto,
    net::node_base& par):
  m_parent{par}
, m_protocol{proto}
, m_phidget{p}
, m_ik{m_phidget->impl()}
{
  m_ik->set_sensor_change([=] (int s, int v)
  {
    send(value());
  });

}

phidget_address::~phidget_address()
{
  m_ik->set_sensor_change({});
}

net::node_base&phidget_address::getNode() const
{
  return m_parent;
}

void phidget_address::pull_value()
{
}

value phidget_address::value() const
{
  std::vector<ossia::value> v;
  int N = m_ik->get_sensor_count();
  for(int i = 0; i < N; i++)
  {
    v.push_back(m_ik->get_sensor_value(i));
  }
  return v;
}

net::address_base&phidget_address::push_value(const ossia::value&)
{
  return *this;
}

net::address_base&phidget_address::push_value(ossia::value&&)
{
  return *this;
}

net::address_base&phidget_address::push_value()
{
  return *this;
}

net::address_base&phidget_address::set_value(const ossia::value&)
{
  return *this;
}

net::address_base&phidget_address::set_value(ossia::value&&)
{
  return *this;
}

val_type phidget_address::get_value_type() const
{
  return ossia::val_type::TUPLE;
}

net::address_base&phidget_address::set_value_type(val_type)
{
  return *this;
}

access_mode phidget_address::get_access() const
{
  return ossia::access_mode::GET;
}

net::address_base&phidget_address::set_access(access_mode)
{
  return *this;
}

const domain&phidget_address::get_domain() const
{
  return m_domain;
}

net::address_base&phidget_address::set_domain(const domain&)
{
  return *this;
}

bounding_mode phidget_address::get_bounding() const
{
  return ossia::bounding_mode::FREE;
}

net::address_base&phidget_address::set_bounding(bounding_mode)
{
  return *this;
}

repetition_filter phidget_address::get_repetition_filter() const
{
  return ossia::repetition_filter::OFF;
}

net::address_base&phidget_address::set_repetition_filter(repetition_filter)
{
  return *this;
}


}
