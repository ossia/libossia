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
generic_address::generic_address(const ossia::net::node_base& node)
  : mNode{node}
  , mProtocol{node.getDevice().getProtocol()}
  , mValueType(ossia::val_type::IMPULSE)
  , mAccessMode(ossia::access_mode::BI)
  , mBoundingMode(ossia::bounding_mode::FREE)
  , mRepetitionFilter(ossia::repetition_filter::OFF)
  , mValue(ossia::Impulse{})
  , mDefault(ossia::Impulse{})
{
}

generic_address::generic_address(
    const generic_address_data& data,
    const ossia::net::node_base& node)
  : mNode{node}
  , mProtocol{node.getDevice().getProtocol()}
  , mValueType(get_value_or(data.type, ossia::val_type::IMPULSE))
  , mAccessMode(get_value_or(data.access, ossia::access_mode::BI))
  , mBoundingMode(get_value_or(data.bounding, ossia::bounding_mode::FREE))
  , mRepetitionFilter(get_value_or(data.repetition_filter, ossia::repetition_filter::OFF))
  , mValue(init_value(mValueType))
  , mDefault(init_value(mValueType))
{
}

generic_address::~generic_address()
{
  callback_container<value_callback>::callbacks_clear();
}

const ossia::net::node_base& generic_address::getNode() const
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
  send(setValueQuiet(val));
  return *this;
}

ossia::value generic_address::setValueQuiet(const value& val)
{
  using namespace ossia;
  if(!val.valid())
    return *this;

  std::unique_lock<std::mutex> lock(mValueMutex);
  // std::cerr << address_string_from_node(*this) << " : " << mValue << " <=== " << val << std::endl;

  // set value querying the value from another address
  auto dest = val.target<Destination>();
  if (dest && mValueType != val_type::DESTINATION)
  {
    const Destination& destination = *dest;

    if (destination.value.get().getValueType() == mValueType)
    {
      mValue = destination.value.get().fetchValue();
    }
    else
    {
      throw invalid_node_error(
            "generic_address::setValue: "
            "setting an address value using a destination "
            "with a bad type address");
      return *this;
    }
  }
  // copy the new value
  else
  {
    if (mValue.v.which() == val.v.which())
    {
      mValue = val;
    }
    else
    {
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

  return mValue;
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
  mNode.getDevice().onAddressModified(*this);
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
    mNode.getDevice().onAddressModified(*this);
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
    mNode.getDevice().onAddressModified(*this);
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
    mNode.getDevice().onAddressModified(*this);
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
    mNode.getDevice().onAddressModified(*this);
  }
  return *this;
}

void generic_address::onFirstCallbackAdded()
{
  mProtocol.observe(*this, true);
}

void generic_address::onRemovingLastCallback()
{
  mProtocol.observe(*this, false);
}

std::vector<std::string> generic_address::getTags() const
{
  return mTags;
}

generic_address& generic_address::setTags(const std::vector<std::string>& v)
{
  mTags = v;
  mNode.getDevice().onAddressModified(*this);
  return *this;
}

std::string generic_address::getDescription() const
{
  return mDescription;
}

generic_address& generic_address::setDescription(const std::string& v)
{
  mDescription = v;
  mNode.getDevice().onAddressModified(*this);
  return *this;
}

value generic_address::getDefaultValue() const
{
  return mDefault;
}

generic_address& generic_address::setDefaultValue(const value& v)
{
  mDefault = v;
  mNode.getDevice().onAddressModified(*this);
  return *this;
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
  mNode.getDevice().onAddressModified(*this);
  return *this;
}
}
}
