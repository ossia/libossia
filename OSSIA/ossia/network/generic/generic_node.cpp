#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <cassert>

namespace impl
{
BasicNode::BasicNode(
    std::string name,
    OSSIA::net::Device& aDevice,
    BasicNode& aParent):
    mName{std::move(name)},
    mDevice{aDevice},
    mParent{&aParent}
{

}

BasicNode::BasicNode(std::string name, OSSIA::net::Device& aDevice):
    mName{std::move(name)},
    mDevice{aDevice}
{

}

BasicNode::~BasicNode()
{
    mChildren.clear();
}

OSSIA::net::Node& BasicNode::setName(std::string name)
{
    std::swap(mName, name);

    // notify observers
    mDevice.onNodeRenamed(*this, name);

    return *this;
}

OSSIA::net::Address* BasicNode::getAddress() const
{
    return mAddress.get();
}

OSSIA::net::Address* BasicNode::createAddress(OSSIA::Type type)
{
    // clear former address
    removeAddress();

    // edit new address
    mAddress = std::make_unique<impl::BasicAddress>(*this);

    // set type
    mAddress->setValueType(type);

    // notify observers
    mDevice.onAddressCreated(*mAddress);

    return mAddress.get();
}

bool BasicNode::removeAddress()
{
    // use the device protocol to stop address value observation
    if (mAddress)
    {
        // notify observers
        mDevice.onAddressRemoving(*mAddress);

        auto& device = getDevice();
        device.getProtocol().observe(*mAddress, false);


        mAddress.reset();

        return true;
    }

    return false;
}

}
