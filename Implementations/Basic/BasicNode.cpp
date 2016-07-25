#include "BasicNode.h"
#include "BasicDevice.h"
#include "Protocol.hpp"
#include <Editor/Value/Value.h>
#include "BasicAddress.h"
#include <cassert>

namespace impl
{
BasicNode::BasicNode(
        std::shared_ptr<OSSIA::v2::Device> aDevice,
        std::shared_ptr<BasicNode> aParent) :
    mDevice(aDevice),
    mParent(aParent)
{
}

BasicNode::~BasicNode()
{
    m_children.clear();
}


shared_ptr<OSSIA::v2::Device> BasicNode::getDevice() const
{
    return mDevice.lock();
}

std::shared_ptr<OSSIA::v2::Node> BasicNode::getParent() const
{
    return mParent.lock();
}

std::shared_ptr<OSSIA::v2::Node> BasicNode::getThis()
{
    return shared_from_this();
}

std::string BasicNode::getName() const
{
    return mName;
}

OSSIA::v2::Node& BasicNode::setName(std::string name)
{
    std::swap(mName, name);
    // notify observers
    send(*this, name, NodeChange::RENAMED);

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
    send(*this, getName(), NodeChange::ADDRESS_CREATED);

    return mAddress.get();
}

bool BasicNode::removeAddress()
{
    // use the device protocol to stop address value observation
    if (mAddress)
    {
        if (auto device = getDevice())
        {
            device->getProtocol().observeAddressValue(*mAddress, false);
        }

        mAddress.reset();

        // notify observers
        send(*this, getName(), NodeChange::ADDRESS_REMOVED);

        return true;
    }

    return false;
}

Container<OSSIA::v2::Node>::iterator BasicNode::emplace(Container<OSSIA::v2::Node>::const_iterator pos, string name)
{
    if (name.empty())
        throw runtime_error("can't create a node with empty name");


    return Container<OSSIA::v2::Node>::iterator();
}

Container<OSSIA::v2::Node>::iterator BasicNode::emplace(Container<OSSIA::v2::Node>::const_iterator pos,
                                                         const string& name,
                                                         Type type,
                                                         AccessMode access,
                                                         const std::shared_ptr<Domain>& domain,
                                                         BoundingMode bm,
                                                         bool repetitionFilter)
{
    return Container<OSSIA::v2::Node>::iterator();
}

Container<OSSIA::v2::Node>::iterator BasicNode::insert(Container<OSSIA::v2::Node>::const_iterator pos, std::shared_ptr<OSSIA::v2::Node> node, std::string name)
{
    assert(!name.empty());

    return m_children.insert(pos, node);
}

Container<OSSIA::v2::Node>::iterator BasicNode::erase(Container<OSSIA::v2::Node>::const_iterator requested_it)
{
    return Container<OSSIA::v2::Node>::iterator();
}

OSSIA::v2::Node::iterator BasicNode::addCallback(OSSIA::v2::NodeChangeCallback callback)
{
    return OSSIA::v2::Node::iterator();
}

void BasicNode::removeCallback(Node::iterator callback)
{
    CallbackContainer::removeCallback(callback);

    if (callbacks().size() == 0)
    {
        // stop children changes observation
        for (auto child : children())
        {
            Node::iterator callbackIndex = mChildNodeChangeCallbackIndexes.find(child)->second;
            child->removeCallback(callbackIndex);
        }

        mChildNodeChangeCallbackIndexes.clear();
    }
}

}
