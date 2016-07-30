#include <ossia/network/generic/generic_address.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/network_logger.hpp>

#include <ossia/network/base/protocol.hpp>
#include <map>
#include <iostream>

namespace impl {

BasicAddress::BasicAddress(
        const ossia::net::node& node) :
    mNode{node},
    mProtocol{node.getDevice().getProtocol()},
    mValue(ossia::Impulse{}),
    mValueType(ossia::Type::IMPULSE),
    mAccessMode(ossia::AccessMode::BI),
    mBoundingMode(ossia::BoundingMode::FREE),
    mRepetitionFilter(ossia::RepetitionFilter::OFF)
{
    mTextualAddress = ossia::net::getAddressFromNode(mNode);
}

BasicAddress::~BasicAddress()
{
  mCallbacks.clear();
}

const ossia::net::node& BasicAddress::getNode() const
{
    return mNode;
}

void BasicAddress::pullValue()
{
    mProtocol.pull(*this);
}

ossia::net::address& BasicAddress::pushValue(const ossia::value& value)
{
    setValue(value);

    mProtocol.push(*this);

    return *this;
}

ossia::net::address& BasicAddress::pushValue()
{
    mProtocol.push(*this);

    return *this;
}

const ossia::value& BasicAddress::getValue() const
{
    return mValue;
}

ossia::value BasicAddress::cloneValue(ossia::destination_index index) const
{
    using namespace ossia;
    std::lock_guard<std::mutex> lock(mValueMutex);

    if (mValue.valid())
    {
        if (index.empty() || mValueType != Type::TUPLE)
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

ossia::net::address& BasicAddress::setValue(const ossia::value& value)
{
    using namespace ossia;
    std::unique_lock<std::mutex> lock(mValueMutex);

    // set value querying the value from another address
    auto dest = value.try_get<Destination>();
    if (dest && mValueType != Type::DESTINATION)
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
          throw std::runtime_error("setting an address value using a destination with a bad type address");
      }
      else
      {
        throw std::runtime_error("setting an address value using a destination without address");
      }
    }
    // copy the new value
    else
    {
        if(mValue.v.which() != value.v.which())
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

ossia::Type BasicAddress::getValueType() const
{
    return mValueType;
}

ossia::net::address& BasicAddress::setValueType(ossia::Type type)
{
    mValueType = type;

    mValue = initValue(type);
    // TODO convert domain if applicable

    return *this;
}

ossia::AccessMode BasicAddress::getAccessMode() const
{
    return mAccessMode;
}

ossia::net::address& BasicAddress::setAccessMode(ossia::AccessMode accessMode)
{
    mAccessMode = accessMode;
    return *this;
}

const ossia::net::Domain& BasicAddress::getDomain() const
{
    return mDomain;
}

ossia::net::address& BasicAddress::setDomain(const ossia::net::Domain& domain)
{
    mDomain = domain;
    return *this;
}

ossia::BoundingMode BasicAddress::getBoundingMode() const
{
    return mBoundingMode;
}

ossia::net::address& BasicAddress::setBoundingMode(ossia::BoundingMode boundingMode)
{
    mBoundingMode = boundingMode;
    return *this;
}

ossia::RepetitionFilter BasicAddress::getRepetitionFilter() const
{
    return mRepetitionFilter;
}

ossia::net::address& BasicAddress::setRepetitionFilter(ossia::RepetitionFilter repetitionFilter)
{
    mRepetitionFilter = repetitionFilter;

    return *this;
}

void BasicAddress::onFirstCallbackAdded()
{
  mProtocol.observe(*this, true);
}

void BasicAddress::onRemovingLastCallback()
{
  mProtocol.observe(*this, false);
}
}
