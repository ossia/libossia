// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/parameter_data.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/domain/domain_conversion.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/value/value_conversion.hpp>

namespace ossia::net
{
struct dummy_lock
{
  template <typename T>
  explicit dummy_lock(T&&)
  {
  }
};
#if defined(OSSIA_PARAMETER_VALUE_SINGLETHREAD)
using value_lock_t = dummy_lock;
#else
using value_lock_t = ossia::lock_t;
#endif

generic_parameter::generic_parameter(ossia::net::node_base& node)
    : ossia::net::parameter_base{node}
    , m_protocol{node.get_device().get_protocol()}
    , m_valueType(ossia::val_type::IMPULSE)
    , m_accessMode(ossia::access_mode::BI)
    , m_boundingMode(ossia::bounding_mode::FREE)
    , m_value(ossia::impulse{})
{
}

generic_parameter::generic_parameter(
    const parameter_data& data, ossia::net::node_base& node)
    : ossia::net::parameter_base{node}
    , m_protocol{node.get_device().get_protocol()}
    , m_valueType(ossia::underlying_type(data.type))
    , m_accessMode(get_value_or(data.access, ossia::access_mode::BI))
    , m_boundingMode(get_value_or(data.bounding, ossia::bounding_mode::FREE))
    , m_value(data.value)
    , m_domain(get_value_or(data.domain, ossia::domain{}))
{
  m_unit = data.unit;
  m_repetitionFilter = get_value_or(data.rep_filter, ossia::repetition_filter::OFF);
  update_parameter_type(data.type, *this);
}

generic_parameter::~generic_parameter()
{
  callback_container<value_callback>::callbacks_clear();
}

void generic_parameter::pull_value()
{
  m_protocol.pull(*this);
}

std::future<void> generic_parameter::pull_value_async()
{
  return m_protocol.pull_async(*this);
}

void generic_parameter::request_value()
{
  m_protocol.request(*this);
}

ossia::net::generic_parameter& generic_parameter::push_value(const ossia::value& value)
{
  if(auto res = set_value(value); res.valid())
    m_protocol.push(*this, std::move(res));

  return *this;
}

ossia::net::generic_parameter& generic_parameter::push_value(ossia::value&& value)
{
  if(auto res = set_value(std::move(value)); res.valid())
    m_protocol.push(*this, std::move(res));

  return *this;
}

ossia::net::generic_parameter& generic_parameter::push_value()
{
  m_protocol.push(*this, value());

  return *this;
}

const ossia::value& generic_parameter::getValue() const
{
  return m_value;
}

ossia::value generic_parameter::value() const
{
  value_lock_t lock(m_valueMutex);

  return m_value;
}

ossia::value generic_parameter::set_value(const ossia::value& val)
{
  ossia::value copy;

  if(val.valid())
  {
    value_lock_t lock(m_valueMutex);
    if(m_value.v.which() == val.v.which())
    {
      // TODO assess whether we would avoid an allocation on the return
      // if doing copy = std::move(m_previousValue)
      m_previousValue = std::move(m_value); // TODO also implement me for MIDI
      m_value = val;
      copy = val;
    }
    else
    {
      m_previousValue = std::move(m_value);
      m_value = ossia::convert(val, m_previousValue);
      copy = m_value;
    }
  }
  send(copy);

  return copy;
}

ossia::value generic_parameter::set_value(ossia::value&& val)
{
  using namespace ossia;
  ossia::value copy;
  if(val.valid())
  {
    value_lock_t lock(m_valueMutex);
    if(m_value.v.which() == val.v.which())
    {
      m_previousValue = std::move(m_value); // TODO also implement me for MIDI
      m_value = std::move(val);
      copy = m_value;
    }
    else
    {
      m_previousValue = std::move(m_value);
      m_value = ossia::convert(std::move(val), m_previousValue);
      copy = m_value;
    }
  }

  send(copy);
  return copy;
}

ossia::value generic_parameter::set_value_quiet(const ossia::value& val)
{
  ossia::value copy;

  if(val.valid())
  {
    value_lock_t lock(m_valueMutex);
    if(m_value.v.which() == val.v.which())
    {
      // TODO assess whether we would avoid an allocation on the return
      // if doing copy = std::move(m_previousValue)
      m_previousValue = std::move(m_value); // TODO also implement me for MIDI
      m_value = val;
      copy = val;
    }
    else
    {
      m_previousValue = std::move(m_value);
      m_value = ossia::convert(val, m_previousValue);
      copy = m_value;
    }
  }

  return copy;
}

ossia::value generic_parameter::set_value_quiet(ossia::value&& val)
{
  using namespace ossia;
  ossia::value copy;
  if(val.valid())
  {
    value_lock_t lock(m_valueMutex);
    if(m_value.v.which() == val.v.which())
    {
      m_previousValue = std::move(m_value); // TODO also implement me for MIDI
      m_value = std::move(val);
      copy = m_value;
    }
    else
    {
      m_previousValue = std::move(m_value);
      m_value = ossia::convert(std::move(val), m_previousValue);
      copy = m_value;
    }
  }

  return copy;
}

void generic_parameter::set_value_quiet(const destination& destination)
{
  value_lock_t lock(m_valueMutex);
  if(destination.address().get_value_type() == m_valueType)
  {
    m_previousValue = std::move(m_value); // TODO also implement me for MIDI
    m_value = destination.address().fetch_value();
  }
  else
  {
    ossia_do_throw(
        invalid_node_error,
        "generic_parameter::setValue: "
        "setting a parameter value using a destination "
        "with a bad type address");
    return;
  }
}

ossia::val_type generic_parameter::get_value_type() const noexcept
{
  return m_valueType;
}

ossia::net::generic_parameter& generic_parameter::set_value_type(ossia::val_type type)
{
  if(m_valueType == type)
    return *this;

  {
    value_lock_t lock(m_valueMutex);
    // std::cerr << address_string_from_node(*this) << " TYPE CHANGE : " <<
    // (int) mValueType << " <=== " << (int) type << std::endl;
    m_valueType = type;

    m_value = init_value(type);
    if(m_domain)
    {
      convert_compatible_domain(m_domain, m_valueType);
    }
  }
  m_node.get_device().on_attribute_modified(m_node, std::string(text_value_type()));
  return *this;
}

ossia::access_mode generic_parameter::get_access() const noexcept
{
  return m_accessMode;
}

ossia::net::generic_parameter&
generic_parameter::set_access(ossia::access_mode accessMode)
{
  if(m_accessMode != accessMode)
  {
    m_accessMode = accessMode;
    m_node.get_device().on_attribute_modified(m_node, std::string(text_access_mode()));
  }
  return *this;
}

const ossia::domain& generic_parameter::get_domain() const noexcept
{
  return m_domain;
}

ossia::net::generic_parameter& generic_parameter::set_domain(const ossia::domain& domain)
{
  if(m_domain != domain)
  {
    m_domain = domain;
    convert_compatible_domain(m_domain, m_valueType);

    m_node.get_device().on_attribute_modified(m_node, std::string(text_domain()));
  }
  return *this;
}

ossia::bounding_mode generic_parameter::get_bounding() const noexcept
{
  return m_boundingMode;
}

ossia::net::generic_parameter&
generic_parameter::set_bounding(ossia::bounding_mode boundingMode)
{
  if(m_boundingMode != boundingMode && m_valueType != ossia::val_type::BOOL)
  {
    m_boundingMode = boundingMode;
    m_node.get_device().on_attribute_modified(m_node, std::string(text_bounding_mode()));
  }
  return *this;
}

bool generic_parameter::filter_value(const ossia::value& val) const noexcept
{
  return m_disabled || m_muted
         || (get_repetition_filter() == ossia::repetition_filter::ON
             && val == m_previousValue);
}

void generic_parameter::on_first_callback_added()
{
  m_protocol.observe(*this, true);
}

void generic_parameter::on_removing_last_callback()
{
  m_protocol.observe(*this, false);
}

generic_parameter& generic_parameter::set_unit(const unit_t& v)
{
  {
    value_lock_t lock(m_valueMutex);
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
  m_node.get_device().on_attribute_modified(m_node, std::string(text_unit()));
  return *this;
}
}
