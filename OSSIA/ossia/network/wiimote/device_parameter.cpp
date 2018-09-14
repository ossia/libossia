
#include "device_parameter.hpp"

using namespace ossia;

namespace device
{

device_parameter::device_parameter(
    net::node_base& node, const val_type type, const bounding_mode bounding,
    const access_mode access, const domain domain)
    : net::parameter_base{node}
    , m_current_value{}
    , m_type{type}
    , m_bounding{bounding}
    , m_access{access}
    , m_domain{domain}
{
  set_repetition_filter(repetition_filter::ON);
}

device_parameter::~device_parameter()
{
}

void device_parameter::device_value_change_event(const ossia::value& val)
{
  if (val.valid())
  {
    m_current_value = val;
    get_protocol().push(*this);
  }
}

void device_parameter::pull_value()
{
  get_protocol().pull(*this);
}

ossia::value device_parameter::value() const
{
  return m_current_value;
}

net::parameter_base& device_parameter::push_value(const ossia::value& val)
{
  set_value(val);
  get_protocol().push(*this);
  return *this;
}

net::parameter_base& device_parameter::push_value(ossia::value&& val)
{
  return push_value(val);
}

net::parameter_base& device_parameter::push_value()
{
  get_protocol().push(*this);
  return *this;
}

net::parameter_base& device_parameter::set_value(const ossia::value& val)
{
  if (val.valid())
  {
    m_current_value = val;
    send(val);
    device_update_value();
  }

  return *this;
}

net::parameter_base& device_parameter::set_value(ossia::value&& val)
{
  return set_value(val);
}

val_type device_parameter::get_value_type() const
{
  return m_type;
}

access_mode device_parameter::get_access() const
{
  return m_access;
}

const domain& device_parameter::get_domain() const
{
  return m_domain;
}

bounding_mode device_parameter::get_bounding() const
{
  return m_bounding;
}
}