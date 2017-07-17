// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/midi/midi.hpp>
#include <ossia/network/midi/midi_address.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
midi_address::midi_address(address_info info, node_base& parent)
    : m_info{info}
    , m_parent{parent}
    , m_protocol{dynamic_cast<midi_protocol&>(
          parent.get_device().get_protocol())}
    , m_domain{m_info.defaultDomain()}
    , m_type{m_info.matchingType()}
    , m_value{m_info.defaultValue(64)}
{
}

const address_info& midi_address::info() const
{
  return m_info;
}

node_base& midi_address::get_node() const
{
  return m_parent;
}

void midi_address::pull_value()
{
  m_protocol.pull(*this);
}

address_base& midi_address::push_value(const ossia::value& val)
{
  m_value = val;
  m_protocol.push(*this);
  return *this;
}

address_base& midi_address::push_value(ossia::value&& val)
{
  m_value = std::move(val);
  m_protocol.push(*this);
  return *this;
}

address_base& midi_address::push_value()
{
  m_protocol.push(*this);
  return *this;
}

const ossia::value& midi_address::getValue() const
{
  return m_value;
}

ossia::value midi_address::value() const
{
  return m_value;
}

address_base& midi_address::set_value(const ossia::value& v)
{
  if (m_type == v.getType())
    m_value = v;
  else
    m_value = ossia::convert(v, m_type);

  send(m_value);
  return *this;
}

address_base& midi_address::set_value(ossia::value&& v)
{
  if (m_type == v.getType())
    m_value = std::move(v);
  else
    m_value = ossia::convert(std::move(v), m_type);

  send(m_value);
  return *this;
}

val_type midi_address::get_value_type() const
{
  return m_type;
}

address_base& midi_address::set_value_type(val_type)
{
  return *this;
}

access_mode midi_address::get_access() const
{
  return ossia::access_mode::BI;
}

address_base& midi_address::set_access(access_mode)
{
  return *this;
}

const ossia::domain& midi_address::get_domain() const
{
  return m_domain;
}

address_base& midi_address::set_domain(const ossia::domain&)
{
  return *this;
}

bounding_mode midi_address::get_bounding() const
{
  return ossia::bounding_mode::CLIP;
}

address_base& midi_address::set_bounding(bounding_mode)
{
  return *this;
}

repetition_filter midi_address::get_repetition_filter() const
{
  return ossia::repetition_filter::OFF;
}

address_base& midi_address::set_repetition_filter(repetition_filter)
{
  return *this;
}

void midi_address::on_first_callback_added()
{
  m_protocol.observe(*this, true);
}

void midi_address::on_removing_last_callback()
{
  m_protocol.observe(*this, false);
}

void midi_address::value_callback(const ossia::value& val)
{
  this->set_value(val);
}
}
}
}
