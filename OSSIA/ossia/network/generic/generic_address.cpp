#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia/network/base/protocol.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <iostream>
#include <map>

namespace ossia
{
namespace net
{
generic_address::generic_address(ossia::net::node_base& node)
  : mNode{node}
  , mProtocol{node.getDevice().getProtocol()}
  , mValueType(ossia::val_type::IMPULSE)
  , mAccessMode(ossia::access_mode::BI)
  , mBoundingMode(ossia::bounding_mode::FREE)
  , mRepetitionFilter(ossia::repetition_filter::OFF)
  , mValue(ossia::impulse{})
{
}

generic_address::generic_address(
    const generic_address_data& data,
    ossia::net::node_base& node)
  : mNode{node}
  , mProtocol{node.getDevice().getProtocol()}
  , mValueType(get_value_or(data.type, ossia::val_type::IMPULSE))
  , mAccessMode(get_value_or(data.access, ossia::access_mode::BI))
  , mBoundingMode(get_value_or(data.bounding, ossia::bounding_mode::FREE))
  , mRepetitionFilter(get_value_or(data.repetition_filter, ossia::repetition_filter::OFF))
  , mValue(init_value(mValueType))
{
}

generic_address::~generic_address()
{
  callback_container<value_callback>::callbacks_clear();
}

ossia::net::node_base& generic_address::getNode() const
{
  return mNode;
}

void generic_address::pullValue()
{
  mProtocol.pull(*this);
}

std::future<void> generic_address::pullValueAsync()
{
  return mProtocol.pullAsync(*this);
}

void generic_address::requestValue()
{
  mProtocol.request(*this);
}

ossia::net::generic_address& generic_address::pushValue(const ossia::value& value)
{
  setValue(value);

  mProtocol.push(*this);

  return *this;
}

ossia::net::generic_address& generic_address::pushValue()
{
  mProtocol.push(*this);

  return *this;
}

const ossia::value& generic_address::getValue() const
{
  return mValue;
}

ossia::value generic_address::cloneValue() const
{
  std::lock_guard<std::mutex> lock(mValueMutex);

  return mValue;
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

  std::lock_guard<std::mutex> lock(mValueMutex);
  // std::cerr << address_string_from_node(*this) << " : " << mValue << " <=== " << val << std::endl;

  // set value querying the value from another address
  auto dest = val.target<Destination>();
  if (dest && mValueType != val_type::DESTINATION)
  {
    const Destination& destination = *dest;

    if (destination.value.get().getValueType() == mValueType)
    {
      mPreviousValue = std::move(mValue); // TODO also implement me for MIDI
      mValue = destination.value.get().fetchValue();
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
    if (mValue.v.which() == val.v.which())
    {
      mPreviousValue = std::move(mValue); // TODO also implement me for MIDI
      mValue = val;
    }
    else
    {
      mPreviousValue = mValue;
      mValue = ossia::convert(val, mValue.getType());

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
  return mValueType;
}

ossia::net::generic_address& generic_address::setValueType(ossia::val_type type)
{
  {
    std::lock_guard<std::mutex> lock(mValueMutex);
    // std::cerr << address_string_from_node(*this) << " TYPE CHANGE : " << (int) mValueType << " <=== " << (int) type << std::endl;
    mValueType = type;

    mValue = init_value(type);
    /*
  if(mDomain)
    mDomain = convert_domain(mDomain, mValueType);
  */
  }
  mNode.getDevice().onAttributeModified(mNode, text_value_type());
  return *this;
}

ossia::access_mode generic_address::getAccessMode() const
{
  return mAccessMode;
}

ossia::net::generic_address&
generic_address::setAccessMode(ossia::access_mode accessMode)
{
  if(mAccessMode != accessMode)
  {
    mAccessMode = accessMode;
    mNode.getDevice().onAttributeModified(mNode, text_access_mode());
  }
  return *this;
}

const ossia::net::domain& generic_address::getDomain() const
{
  return mDomain;
}

ossia::net::generic_address&
generic_address::setDomain(const ossia::net::domain& domain)
{
  if(mDomain != domain)
  {
    // TODO we should check that the domain is correct
    // for the type of the value.
    mDomain = domain;
    mNode.getDevice().onAttributeModified(mNode, text_domain());
  }
  return *this;
}

ossia::bounding_mode generic_address::getBoundingMode() const
{
  return mBoundingMode;
}

ossia::net::generic_address&
generic_address::setBoundingMode(ossia::bounding_mode boundingMode)
{
  if(mBoundingMode != boundingMode)
  {
    mBoundingMode = boundingMode;
    mNode.getDevice().onAttributeModified(mNode, text_bounding_mode());
  }
  return *this;
}

ossia::repetition_filter generic_address::getRepetitionFilter() const
{
  return mRepetitionFilter;
}

ossia::net::generic_address&
generic_address::setRepetitionFilter(ossia::repetition_filter repetitionFilter)
{
  if(mRepetitionFilter != repetitionFilter)
  {
    mRepetitionFilter = repetitionFilter;
    mNode.getDevice().onAttributeModified(mNode, text_repetition_filter());
  }
  return *this;
}

bool generic_address::filterRepetition(const value& val) const
{
  return getRepetitionFilter() == ossia::repetition_filter::ON
      && val == mPreviousValue;
}

void generic_address::onFirstCallbackAdded()
{
  mProtocol.observe(*this, true);
}

void generic_address::onRemovingLastCallback()
{
  mProtocol.observe(*this, false);
}

unit_t generic_address::getUnit() const
{
  return mUnit;
}

generic_address& generic_address::setUnit(const unit_t& v)
{
  {
    std::lock_guard<std::mutex> lock(mValueMutex);
    mUnit = v;

    // update the type to match the unit.
    if(v)
    {
      auto vt = ossia::matching_type(v);
      if(vt != ossia::val_type::IMPULSE)
      {
        mValueType = vt;
        mValue = ossia::convert(mValue, mValueType);

        /*
      if(mDomain)
        mDomain = convert_domain(mDomain, mValueType);
      */
      }
    }
  }
  mNode.getDevice().onAttributeModified(mNode, text_unit());
  return *this;
}
}
}
