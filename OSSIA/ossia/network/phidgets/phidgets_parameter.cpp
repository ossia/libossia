// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "phidgets_parameter.hpp"
#include "phidgets_device.hpp"
#include "phidgets_node.hpp"
#include "phidgets_protocol.hpp"

namespace ossia
{

phidget_parameter::phidget_parameter(
    ppp::phidget_ptr p, phidget_protocol& proto, net::node_base& par)
    : ossia::net::parameter_base{par}
    , m_protocol{proto}
    , m_phidget{p}
    , m_ik{m_phidget->impl()}
{
  m_ik->set_sensor_change([=](int s, int v) { send(value()); });
}

phidget_parameter::~phidget_parameter()
{
  m_ik->set_sensor_change({});
}

net::node_base& phidget_parameter::get_node() const
{
  return m_parent;
}

void phidget_parameter::pull_value()
{
}

value phidget_parameter::value() const
{
  std::vector<ossia::value> v;
  int N = m_ik->get_sensor_count();
  for (int i = 0; i < N; i++)
  {
    v.push_back(m_ik->get_sensor_value(i));
  }
  return v;
}

net::parameter_base& phidget_parameter::push_value(const ossia::value&)
{
  return *this;
}

net::parameter_base& phidget_parameter::push_value(ossia::value&&)
{
  return *this;
}

net::parameter_base& phidget_parameter::push_value()
{
  return *this;
}

net::parameter_base& phidget_parameter::set_value(const ossia::value&)
{
  return *this;
}

net::parameter_base& phidget_parameter::set_value(ossia::value&&)
{
  return *this;
}

val_type phidget_parameter::get_value_type() const
{
  return ossia::val_type::LIST;
}

net::parameter_base& phidget_parameter::set_value_type(val_type)
{
  return *this;
}

access_mode phidget_parameter::get_access() const
{
  return ossia::access_mode::GET;
}

net::parameter_base& phidget_parameter::set_access(access_mode)
{
  return *this;
}

const domain& phidget_parameter::get_domain() const
{
  return m_domain;
}

net::parameter_base& phidget_parameter::set_domain(const domain&)
{
  return *this;
}

bounding_mode phidget_parameter::get_bounding() const
{
  return ossia::bounding_mode::FREE;
}

net::parameter_base& phidget_parameter::set_bounding(bounding_mode)
{
  return *this;
}

repetition_filter phidget_parameter::get_repetition_filter() const
{
  return ossia::repetition_filter::OFF;
}

net::parameter_base& phidget_parameter::set_repetition_filter(repetition_filter)
{
  return *this;
}
}
