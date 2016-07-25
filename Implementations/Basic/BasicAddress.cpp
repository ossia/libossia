#include "BasicAddress.h"
#include <Editor/Value/Value.h>
#include "Network/NetworkLogger.h"

#include "Protocol.hpp"
#include <map>
#include <iostream>

namespace impl {

BasicAddress::BasicAddress(const OSSIA::v2::Node& node) :
    mNode(node),
    mValue(Impulse{}),
    mValueType(Type::IMPULSE),
    mAccessMode(AccessMode::BI),
    mBoundingMode(BoundingMode::FREE),
    mRepetitionFilter(false)
{
    // To set-up the protocol cache.
    // Note : the cache works because the nodes cannot change parents.
    getProtocol();
    mTextualAddress = OSSIA::v2::getAddressFromNode(mNode);
}

BasicAddress::~BasicAddress()
{
}

const OSSIA::v2::Node& BasicAddress::getNode() const
{
    return mNode;
}

void BasicAddress::pullValue()
{
    // use the device protocol to pull address value
    getProtocol().pullAddressValue(*this);
}

OSSIA::v2::Address& BasicAddress::pushValue(const OSSIA::Value& value)
{
    setValue(value);

    // use the device protocol to push address value
    getProtocol().pushAddressValue(*this);

    return *this;
}

OSSIA::v2::Address& BasicAddress::pushValue()
{
    // use the device protocol to push address value
    getProtocol().pushAddressValue(*this);

    return *this;
}

const OSSIA::Value& BasicAddress::getValue() const
{
    return mValue;
}

OSSIA::Value BasicAddress::cloneValue(DestinationIndex index) const
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
            vector<Value> values;
            values.reserve(index.size());

            for (char i : index)
            {
                values.push_back(tuple.value[i]);
            }

            return Tuple(std::move(values));
        }
    }
    else
    {
        throw runtime_error("cloning null value");
    }
}

OSSIA::v2::Address& BasicAddress::setValue(const Value& value)
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
        throw runtime_error("setting an address value using a destination with a bad type address");
    }
    else
    {
      throw runtime_error("setting an address value using a destination without address");
    }
    */
    }

    // copy the new value
    else
    {
        if(mValue.v.which() != value.v.which())
        {
            mValueType = value.getType();
        }

        mValue = value;
    }

    return *this;
}

Type BasicAddress::getValueType() const
{
    return mValueType;
}

OSSIA::v2::Address& BasicAddress::setValueType(Type type)
{
    mValueType = type;

    mValue = initValue(type);

    return *this;
}

AccessMode BasicAddress::getAccessMode() const
{
    return mAccessMode;
}

OSSIA::v2::Address& BasicAddress::setAccessMode(AccessMode accessMode)
{
    mAccessMode = accessMode;
    return *this;
}

const std::shared_ptr<OSSIA::Domain> & BasicAddress::getDomain() const
{
    return mDomain;
}

OSSIA::v2::Address& BasicAddress::setDomain(std::shared_ptr<OSSIA::Domain> domain)
{
    mDomain = domain;
    return *this;
}

BoundingMode BasicAddress::getBoundingMode() const
{
    return mBoundingMode;
}

OSSIA::v2::Address& BasicAddress::setBoundingMode(OSSIA::BoundingMode boundingMode)
{
    mBoundingMode = boundingMode;
    return *this;
}

bool BasicAddress::getRepetitionFilter() const
{
    return mRepetitionFilter;
}

OSSIA::v2::Address& BasicAddress::setRepetitionFilter(bool repetitionFilter)
{
    mRepetitionFilter = repetitionFilter;

    return *this;
}

OSSIA::v2::Address::iterator BasicAddress::addCallback(OSSIA::v2::ValueCallback callback)
{
    auto it = CallbackContainer::addCallback(std::move(callback));

    if (callbacks().size() == 1)
    {
        getProtocol().observeAddressValue(*this, true);
    }

    return it;
}

void BasicAddress::removeCallback(Address::iterator callback)
{
    CallbackContainer::removeCallback(callback);

    if (callbacks().size() == 0)
    {
        // use the device protocol to stop address value observation
        getProtocol().observeAddressValue(*this, false);
    }
}


OSSIA::v2::Protocol& getDummyProtocol()
{
    struct DummyProtocol : public OSSIA::v2::Protocol
    {
            bool pullAddressValue(OSSIA::v2::Address&) const override { return true; }
            bool pushAddressValue(const OSSIA::v2::Address&) const override { return true; }
            bool observeAddressValue(OSSIA::v2::Address&, bool) const override { return false; }
            bool updateChildren(OSSIA::v2::Node&) const override { return false; }
    };

    static DummyProtocol proto;
    return proto;
}

OSSIA::v2::Protocol& BasicAddress::getProtocol() const
{
    if(auto cached_proto = mProtocolCache)
    {
        return *cached_proto;
    }
    else
    {
        auto dev = mNode.getDevice();
        if(!dev)
            return getDummyProtocol();
        auto& proto = dev->getProtocol();

        // Save in the cache
        mProtocolCache = &proto;
        return proto;
    }
}

}
