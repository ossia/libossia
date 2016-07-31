#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia/network/base/protocol.hpp>
#include <iostream>
#include <map>

namespace ossia
{
namespace net
{
generic_address::generic_address(const ossia::net::node_base& node)
    : mNode{node}
    , mProtocol{node.getDevice().getProtocol()}
    , mValue(ossia::Impulse{})
    , mValueType(ossia::val_type::IMPULSE)
    , mAccessMode(ossia::access_mode::BI)
    , mBoundingMode(ossia::bounding_mode::FREE)
    , mRepetitionFilter(ossia::repetition_filter::OFF)
{
  mTextualAddress = ossia::net::getAddressFromNode(mNode);
}

generic_address::~generic_address()
{
  mCallbacks.clear();
}

const ossia::net::node_base& generic_address::getNode() const
{
  return mNode;
}

void generic_address::pullValue()
{
  mProtocol.pull(*this);
}

ossia::net::address_base& generic_address::pushValue(const ossia::value& value)
{
  setValue(value);

  mProtocol.push(*this);

  return *this;
}

ossia::net::address_base& generic_address::pushValue()
{
  mProtocol.push(*this);

  return *this;
}

const ossia::value& generic_address::getValue() const
{
  return mValue;
}

ossia::value generic_address::cloneValue(ossia::destination_index index) const
{
  using namespace ossia;
  std::lock_guard<std::mutex> lock(mValueMutex);

  if (mValue.valid())
  {
    if (index.empty() || mValueType != val_type::TUPLE)
    {
      return mValue;
    }
    else if (index.size() == 1)
    {
      // clone value from tuple element at index
      const auto& tuple = mValue.get<Tuple>();
      return tuple.value[index[0]];
    }
    else
    {
      // create a new tuple from tuple's values at index
      const auto& tuple = mValue.get<Tuple>();
      Tuple values;
      values.value.reserve(index.size());

      for (char i : index)
      {
        values.value.push_back(tuple.value[i]);
      }

      return values;
    }
  }
  else
  {
    throw std::runtime_error("cloning null value");
  }
}

ossia::net::address_base& generic_address::setValue(const ossia::value& value)
{
  using namespace ossia;
  std::unique_lock<std::mutex> lock(mValueMutex);

  // set value querying the value from another address
  auto dest = value.try_get<Destination>();
  if (dest && mValueType != val_type::DESTINATION)
  {
    const Destination& destination = *dest;
    auto address = destination.value->getAddress();

    if (address)
    {
      if (address->getValueType() == mValueType)
      {
        address->pullValue();
        mValue = address->cloneValue();
      }
      else
        throw std::runtime_error(
            "setting an address value using a destination "
            "with a bad type address");
    }
    else
    {
      throw std::runtime_error(
          "setting an address value using a destination without address");
    }
  }
  // copy the new value
  else
  {
    if (mValue.v.which() != value.v.which())
    {
      mValueType = value.getType();
      // TODO convert domain if applicable
    }

    mValue = value;
  }

  auto clone = mValue;
  lock.unlock();
  send(clone);
  return *this;
}

ossia::val_type generic_address::getValueType() const
{
  return mValueType;
}

ossia::net::address_base& generic_address::setValueType(ossia::val_type type)
{
  mValueType = type;

  mValue = initValue(type);
  // TODO convert domain if applicable

  return *this;
}

ossia::access_mode generic_address::getAccessMode() const
{
  return mAccessMode;
}

ossia::net::address_base&
generic_address::setAccessMode(ossia::access_mode accessMode)
{
  mAccessMode = accessMode;
  return *this;
}

const ossia::net::domain& generic_address::getDomain() const
{
  return mDomain;
}

ossia::net::address_base&
generic_address::setDomain(const ossia::net::domain& domain)
{
  mDomain = domain;
  return *this;
}

ossia::bounding_mode generic_address::getBoundingMode() const
{
  return mBoundingMode;
}

ossia::net::address_base&
generic_address::setBoundingMode(ossia::bounding_mode boundingMode)
{
  mBoundingMode = boundingMode;
  return *this;
}

ossia::repetition_filter generic_address::getRepetitionFilter() const
{
  return mRepetitionFilter;
}

ossia::net::address_base&
generic_address::setRepetitionFilter(ossia::repetition_filter repetitionFilter)
{
  mRepetitionFilter = repetitionFilter;

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
}
}
