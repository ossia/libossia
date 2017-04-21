#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia/network/base/protocol.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>

namespace ossia
{
namespace net
{

generic_address::generic_address(ossia::net::node_base& node)
  : m_node{node}
  , m_protocol{node.getDevice().getProtocol()}
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
  , m_protocol{node.getDevice().getProtocol()}
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

void generic_address::pullValue()
{
  m_protocol.pull(*this);
}

std::future<void> generic_address::pullValueAsync()
{
  return m_protocol.pullAsync(*this);
}

void generic_address::requestValue()
{
  m_protocol.request(*this);
}

ossia::net::generic_address& generic_address::pushValue(const ossia::value& value)
{
  setValue(value);

  m_protocol.push(*this);

  return *this;
}

ossia::net::generic_address& generic_address::pushValue()
{
  m_protocol.push(*this);

  return *this;
}

const ossia::value& generic_address::getValue() const
{
  return m_value;
}

ossia::value generic_address::cloneValue() const
{
  lock_t lock(m_valueMutex);

  return m_value;
}

ossia::net::generic_address& generic_address::setValue(const ossia::value& val)
{
  setValueQuiet(val);
  send(cloneValue());
  return *this;
}

void generic_address::setValueQuiet(const value& val)
{
  using namespace ossia;
  if(!val.valid())
    return;

  lock_t lock(m_valueMutex);
  // std::cerr << address_string_from_node(*this) << " : " << mValue << " <=== " << val << std::endl;

  // set value querying the value from another address
  auto dest = val.target<Destination>();
  if (dest && m_valueType != val_type::DESTINATION)
  {
    const Destination& destination = *dest;

    if (destination.value.get().getValueType() == m_valueType)
    {
      m_previousValue = std::move(m_value); // TODO also implement me for MIDI
      m_value = destination.value.get().fetchValue();
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
  // copy the new value
  else
  {
    if (m_value.v.which() == val.v.which())
    {
      m_previousValue = std::move(m_value); // TODO also implement me for MIDI
      m_value = val;
    }
    else
    {
      m_previousValue = m_value;
      m_value = ossia::convert(val, m_value.getType());
      /*
        // Alternative : try to convert to the actual value type.
        // There should be a choice here : for instance we should be able to convert
        // the values coming from the network, but change the type of the values coming from here.
        // std::cerr << address_string_from_node(*this) << " TYPE CHANGE : " << mValue.v.which() << " <=== " << val.v.which() << std::endl;
        mValueType = val.getType();
        if(mDomain)
          mDomain = convert_domain(mDomain, mValueType);
        */
    }
  }

  // TODO clamping the input implies ensuring that
  // mValue = ossia::net::clamp(mDomain, mBoundingMode, mValue);
}

ossia::val_type generic_address::getValueType() const
{
  return m_valueType;
}

ossia::net::generic_address& generic_address::setValueType(ossia::val_type type)
{
  {
    lock_t lock(m_valueMutex);
    // std::cerr << address_string_from_node(*this) << " TYPE CHANGE : " << (int) mValueType << " <=== " << (int) type << std::endl;
    m_valueType = type;

    m_value = init_value(type);
    /*
  if(mDomain)
    mDomain = convert_domain(mDomain, mValueType);
  */
  }
  m_node.getDevice().onAttributeModified(m_node, text_value_type());
  return *this;
}

ossia::access_mode generic_address::getAccessMode() const
{
  return m_accessMode;
}

ossia::net::generic_address&
generic_address::setAccessMode(ossia::access_mode accessMode)
{
  if(m_accessMode != accessMode)
  {
    m_accessMode = accessMode;
    m_node.getDevice().onAttributeModified(m_node, text_access_mode());
  }
  return *this;
}

const ossia::domain& generic_address::getDomain() const
{
  return m_domain;
}

ossia::net::generic_address&
generic_address::setDomain(const ossia::domain& domain)
{
  if(m_domain != domain)
  {
    // TODO we should check that the domain is correct
    // for the type of the value.
    m_domain = domain;
    m_node.getDevice().onAttributeModified(m_node, text_domain());
  }
  return *this;
}

ossia::bounding_mode generic_address::getBoundingMode() const
{
  return m_boundingMode;
}

ossia::net::generic_address&
generic_address::setBoundingMode(ossia::bounding_mode boundingMode)
{
  if(m_boundingMode != boundingMode)
  {
    m_boundingMode = boundingMode;
    m_node.getDevice().onAttributeModified(m_node, text_bounding_mode());
  }
  return *this;
}

ossia::repetition_filter generic_address::getRepetitionFilter() const
{
  return m_repetitionFilter;
}

ossia::net::generic_address&
generic_address::setRepetitionFilter(ossia::repetition_filter repetitionFilter)
{
  if(m_repetitionFilter != repetitionFilter)
  {
    m_repetitionFilter = repetitionFilter;
    m_node.getDevice().onAttributeModified(m_node, text_repetition_filter());
  }
  return *this;
}

bool generic_address::filterRepetition(const value& val) const
{
  return getRepetitionFilter() == ossia::repetition_filter::ON
      && val == m_previousValue;
}

void generic_address::onFirstCallbackAdded()
{
  m_protocol.observe(*this, true);
}

void generic_address::onRemovingLastCallback()
{
  m_protocol.observe(*this, false);
}

unit_t generic_address::getUnit() const
{
  return m_unit;
}

generic_address& generic_address::setUnit(const unit_t& v)
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

        /*
      if(mDomain)
        mDomain = convert_domain(mDomain, mValueType);
      */
      }
    }
  }
  m_node.getDevice().onAttributeModified(m_node, text_unit());
  return *this;
}
}
}
