#include <ossia/network/v2/generic/generic_node.hpp>
#include <ossia/network/v2/generic/generic_device.hpp>
#include <ossia/network/v2/base/protocol.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/v2/generic/generic_address.hpp>
#include <cassert>

namespace impl
{
BasicNode::BasicNode(
    std::string name,
    OSSIA::v2::Device& aDevice,
    BasicNode& aParent):
    mName{std::move(name)},
    mDevice{aDevice},
    mParent{&aParent}
{

}

BasicNode::BasicNode(std::string name, OSSIA::v2::Device& aDevice):
    mName{std::move(name)},
    mDevice{aDevice}
{

}

BasicNode::~BasicNode()
{
    mChildren.clear();
}

OSSIA::v2::Node& BasicNode::setName(std::string name)
{
    std::swap(mName, name);

    // notify observers
    mDevice.onNodeRenamed(*this, name);

    return *this;
}

OSSIA::v2::Address* BasicNode::getAddress() const
{
    return mAddress.get();
}

OSSIA::v2::Address* BasicNode::createAddress(OSSIA::Type type)
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
        device.getProtocol().observeAddressValue(*mAddress, false);


        mAddress.reset();

        return true;
    }

    return false;
}

}
