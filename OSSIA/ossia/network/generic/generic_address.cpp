#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia/network/base/protocol.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain_conversion.hpp>

namespace ossia
{
namespace net
{

generic_address::generic_address(ossia::net::node_base& node)
  : m_node{node}
  , m_protocol{node.get_device().get_protocol()}
  , m_valueType(ossia::val_type::IMPULSE)
  , m_accessMode(ossia::access_mode::BI)
  , m_boundingMode(ossia::bounding_mode::FREE)
  , m_repetitionFilter(ossia::repetition_filter::OFF)
  , m_value(ossia::impulse{})
{
}

generic_address::generic_address(
    const address_data& data,
    ossia::net::node_base& node)
  : m_node{node}
  , m_protocol{node.get_device().get_protocol()}
  , m_valueType(ossia::val_type::IMPULSE)
  , m_accessMode(get_value_or(data.access, ossia::access_mode::BI))
  , m_boundingMode(get_value_or(data.bounding, ossia::bounding_mode::FREE))
  , m_repetitionFilter(get_value_or(data.repetition_filter, ossia::repetition_filter::OFF))
  , m_value(init_value(m_valueType))
{
  update_address_type(data.type, *this);
}

generic_address::~generic_address()
{
  callback_container<value_callback>::callbacks_clear();
}

ossia::net::node_base& generic_address::getNode() const
{
  return m_node;
}

void generic_address::pull_value()
{
  m_protocol.pull(*this);
}

std::future<void> generic_address::pull_value_async()
{
  return m_protocol.pull_async(*this);
}

void generic_address::request_value()
{
  m_protocol.request(*this);
}

ossia::net::generic_address& generic_address::push_value(const ossia::value& value)
{
  set_value(value);

  m_protocol.push(*this);

  return *this;
}

ossia::net::generic_address& generic_address::push_value(ossia::value&& value)
{
  set_value(std::move(value));

  m_protocol.push(*this);

  return *this;
}

ossia::net::generic_address& generic_address::push_value()
{
  m_protocol.push(*this);

  return *this;
}

const ossia::value& generic_address::getValue() const
{
  return m_value;
}

ossia::value generic_address::value() const
{
  lock_t lock(m_valueMutex);

  return m_value;
}

ossia::net::generic_address& generic_address::set_value(const ossia::value& val)
{
  set_value_quiet(val);
  send(value());
  return *this;
}

ossia::net::generic_address& generic_address::set_value(ossia::value&& val)
{
  set_value_quiet(std::move(val));
  send(value());
  return *this;
}

void generic_address::set_value_quiet(const ossia::value& val)
{
  using namespace ossia;
  if(!val.valid())
    return;

  lock_t lock(m_valueMutex);
  if (m_value.v.which() == val.v.which())
  {
    m_previousValue = std::move(m_value); // TODO also implement me for MIDI
    m_value = val;
  }
  else
  {
    m_previousValue = m_value;
    m_value = ossia::convert(val, m_value.getType());
  }
}

void generic_address::set_value_quiet(ossia::value&& val)
{
  using namespace ossia;
  if(!val.valid())
    return;

  lock_t lock(m_valueMutex);
  if (m_value.v.which() == val.v.which())
  {
    m_previousValue = std::move(m_value); // TODO also implement me for MIDI
    m_value = std::move(val);
  }
  else
  {
    m_previousValue = std::move(m_value);
    m_value = ossia::convert(std::move(val), m_previousValue.getType());
  }
}

void generic_address::set_value_quiet(const Destination& destination)
{
  lock_t lock(m_valueMutex);
  if (destination.address().get_value_type() == m_valueType)
  {
    m_previousValue = std::move(m_value); // TODO also implement me for MIDI
    m_value = destination.address().fetch_value();
  }
  else
  {
    throw invalid_node_error(
          "generic_address::setValue: "
          "setting an address value using a destination "
          "with a bad type address");
    return;
  }
}

ossia::val_type generic_address::get_value_type() const
{
  return m_valueType;
}

ossia::net::generic_address& generic_address::set_value_type(ossia::val_type type)
{
  {
    lock_t lock(m_valueMutex);
    // std::cerr << address_string_from_node(*this) << " TYPE CHANGE : " << (int) mValueType << " <=== " << (int) type << std::endl;
    m_valueType = type;

    m_value = init_value(type);
    if(m_domain)
    {
      convert_compatible_domain(m_domain, m_valueType);
    }
  }
  m_node.get_device().on_attribute_modified(m_node, text_value_type());
  return *this;
}

ossia::access_mode generic_address::get_access() const
{
  return m_accessMode;
}

ossia::net::generic_address&
generic_address::set_access(ossia::access_mode accessMode)
{
  if(m_accessMode != accessMode)
  {
    m_accessMode = accessMode;
    m_node.get_device().on_attribute_modified(m_node, text_access_mode());
  }
  return *this;
}

const ossia::domain& generic_address::get_domain() const
{
  return m_domain;
}

ossia::net::generic_address&
generic_address::set_domain(const ossia::domain& domain)
{
  if(m_domain != domain)
  {
    m_domain = domain;
    convert_compatible_domain(m_domain, m_valueType);

    m_node.get_device().on_attribute_modified(m_node, text_domain());
  }
  return *this;
}

ossia::bounding_mode generic_address::get_bounding() const
{
  return m_boundingMode;
}

ossia::net::generic_address&
generic_address::set_bounding(ossia::bounding_mode boundingMode)
{
  if(m_boundingMode != boundingMode)
  {
    m_boundingMode = boundingMode;
    m_node.get_device().on_attribute_modified(m_node, text_bounding_mode());
  }
  return *this;
}

ossia::repetition_filter generic_address::get_repetition_filter() const
{
  return m_repetitionFilter;
}

ossia::net::generic_address&
generic_address::set_repetition_filter(ossia::repetition_filter repetitionFilter)
{
  if(m_repetitionFilter != repetitionFilter)
  {
    m_repetitionFilter = repetitionFilter;
    m_node.get_device().on_attribute_modified(m_node, text_repetition_filter());
  }
  return *this;
}

bool generic_address::filter_repetition(const ossia::value& val) const
{
  return get_repetition_filter() == ossia::repetition_filter::ON
      && val == m_previousValue;
}

void generic_address::on_first_callback_added()
{
  m_protocol.observe(*this, true);
}

void generic_address::on_removing_last_callback()
{
  m_protocol.observe(*this, false);
}

unit_t generic_address::get_unit() const
{
  return m_unit;
}

generic_address& generic_address::set_unit(const unit_t& v)
{
  {
    lock_t lock(m_valueMutex);
    m_unit = v;

    // update the type to match the unit.
    if(v)
    {
      auto vt = ossia::matching_type(v);
      if(vt != ossia::val_type::IMPULSE)
      {
        m_valueType = vt;
        m_value = ossia::convert(m_value, m_valueType);
        if(m_domain)
        {
          convert_compatible_domain(m_domain, m_valueType);
        }
      }
    }
  }
  m_node.get_device().on_attribute_modified(m_node, text_unit());
  return *this;
}

bool generic_address::get_muted() const
{
  return m_muted;
}

generic_address&generic_address::set_muted(bool v)
{
  if(m_muted != v)
  {
    m_muted = v;
    m_node.get_device().on_attribute_modified(m_node, text_muted());
  }
  return *this;
}
}
}
