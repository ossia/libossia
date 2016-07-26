#include "BasicNode.hpp"
#include "BasicDevice.hpp"
#include "Protocol.hpp"
#include <Editor/Value/Value.h>
#include "BasicAddress.hpp"
#include <cassert>

namespace impl
{
BasicNode::BasicNode(string name, v2::Device& aDevice, BasicNode& aParent):
    mName{std::move(name)},
    mDevice{aDevice},
    mParent{&aParent}
{

}

BasicNode::BasicNode(string name, v2::Device& aDevice):
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

OSSIA::v2::Address* BasicNode::createAddress(Type type)
{
    // clear former address
    removeAddress();

    // edit new address
    mAddress = make_unique<impl::BasicAddress>(*this);

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
