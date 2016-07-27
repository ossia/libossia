#include <ossia/network/v2/generic/generic_address.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/network_logger.hpp>

#include <ossia/network/v2/base/protocol.hpp>
#include <map>
#include <iostream>

namespace impl {

BasicAddress::BasicAddress(
        const OSSIA::v2::Node& node) :
    mNode{node},
    mProtocol{node.getDevice().getProtocol()},
    mValue(OSSIA::Impulse{}),
    mValueType(OSSIA::Type::IMPULSE),
    mAccessMode(OSSIA::AccessMode::BI),
    mBoundingMode(OSSIA::BoundingMode::FREE),
    mRepetitionFilter(OSSIA::RepetitionFilter::OFF)
{
    mTextualAddress = OSSIA::v2::getAddressFromNode(mNode);
}

BasicAddress::~BasicAddress()
{
    while(!m_callbacks.empty())
        removeCallback(m_callbacks.begin()); // /!\ O(1) because m_callbacks is a list
}

const OSSIA::v2::Node& BasicAddress::getNode() const
{
    return mNode;
}

void BasicAddress::pullValue()
{
    mProtocol.pullAddressValue(*this);
}

OSSIA::v2::Address& BasicAddress::pushValue(const OSSIA::Value& value)
{
    setValue(value);

    mProtocol.pushAddressValue(*this);

    return *this;
}

OSSIA::v2::Address& BasicAddress::pushValue()
{
    mProtocol.pushAddressValue(*this);

    return *this;
}

const OSSIA::Value& BasicAddress::getValue() const
{
    return mValue;
}

OSSIA::Value BasicAddress::cloneValue(OSSIA::DestinationIndex index) const
{
    using namespace OSSIA;
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

OSSIA::v2::Address& BasicAddress::setValue(const OSSIA::Value& value)
{
    using namespace OSSIA;
    std::lock_guard<std::mutex> lock(mValueMutex);

    // set value querying the value from another address
    auto dest = value.try_get<Destination>();
    if (dest && mValueType != Type::DESTINATION)
    {/*
    auto& destination = *dest;
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
    */
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

    return *this;
}

OSSIA::Type BasicAddress::getValueType() const
{
    return mValueType;
}

OSSIA::v2::Address& BasicAddress::setValueType(OSSIA::Type type)
{
    mValueType = type;

    mValue = initValue(type);
    // TODO convert domain if applicable

    return *this;
}

OSSIA::AccessMode BasicAddress::getAccessMode() const
{
    return mAccessMode;
}

OSSIA::v2::Address& BasicAddress::setAccessMode(OSSIA::AccessMode accessMode)
{
    mAccessMode = accessMode;
    return *this;
}

const OSSIA::v2::Domain& BasicAddress::getDomain() const
{
    return mDomain;
}

OSSIA::v2::Address& BasicAddress::setDomain(const OSSIA::v2::Domain& domain)
{
    mDomain = domain;
    return *this;
}

OSSIA::BoundingMode BasicAddress::getBoundingMode() const
{
    return mBoundingMode;
}

OSSIA::v2::Address& BasicAddress::setBoundingMode(OSSIA::BoundingMode boundingMode)
{
    mBoundingMode = boundingMode;
    return *this;
}

OSSIA::RepetitionFilter BasicAddress::getRepetitionFilter() const
{
    return mRepetitionFilter;
}

OSSIA::v2::Address& BasicAddress::setRepetitionFilter(OSSIA::RepetitionFilter repetitionFilter)
{
    mRepetitionFilter = repetitionFilter;

    return *this;
}

OSSIA::v2::Address::iterator BasicAddress::addCallback(OSSIA::v2::ValueCallback callback)
{
    auto it = CallbackContainer::addCallback(std::move(callback));

    if (callbacks().size() == 1)
    {
        mProtocol.observeAddressValue(*this, true);
    }

    return it;
}

void BasicAddress::removeCallback(Address::iterator callback)
{
    if (callbacks().size() == 1)
    {
        // use the device protocol to stop address value observation
        mProtocol.observeAddressValue(*this, false);
    }

    CallbackContainer::removeCallback(callback);
}
}
