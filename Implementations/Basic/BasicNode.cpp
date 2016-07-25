#include "BasicNode.h"
#include "BasicDevice.h"
#include <Editor/Value/Value.h>
#include "BasicAddress.h"
#include <assert.h>

namespace impl
{
BasicNode::BasicNode(
        std::shared_ptr<OSSIA::v2::Device2> aDevice,
        std::shared_ptr<BasicNode> aParent) :
  mDevice(aDevice),
  mParent(aParent)
{
}

BasicNode::~BasicNode()
{
  m_children.clear();
}


shared_ptr<OSSIA::v2::Device2> BasicNode::getDevice() const
{
  return mDevice.lock();
}

std::shared_ptr<OSSIA::v2::Node2> BasicNode::getParent() const
{
  return mParent.lock();
}

std::shared_ptr<OSSIA::v2::Node2> BasicNode::getThis()
{
  return shared_from_this();
}

std::string BasicNode::getName() const
{
  return mName;
}

OSSIA::v2::Node2& BasicNode::setName(std::string name)
{
  std::swap(mName, name);
  // notify observers
  send(*this, name, NodeChange::RENAMED);

  return *this;
}

OSSIA::v2::Address2* BasicNode::getAddress() const
{
  return mAddress.get();
}

OSSIA::v2::Address2* BasicNode::createAddress(Type type)
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

Container<OSSIA::v2::Node2>::iterator BasicNode::emplace(Container<OSSIA::v2::Node2>::const_iterator pos, string name)
{
  if (name.empty())
    throw runtime_error("can't create a node with empty name");


  return Container<OSSIA::v2::Node2>::iterator();
}

Container<OSSIA::v2::Node2>::iterator BasicNode::emplace(Container<OSSIA::v2::Node2>::const_iterator pos,
                                              const string& name,
                                              Type type,
                                              AccessMode access,
                                              const std::shared_ptr<Domain>& domain,
                                              BoundingMode bm,
                                              bool repetitionFilter)
{
  return Container<OSSIA::v2::Node2>::iterator();
}

Container<OSSIA::v2::Node2>::iterator BasicNode::insert(Container<OSSIA::v2::Node2>::const_iterator pos, std::shared_ptr<OSSIA::v2::Node2> node, std::string name)
{
  assert(!name.empty());

  return m_children.insert(pos, node);
}

Container<OSSIA::v2::Node2>::iterator BasicNode::erase(Container<OSSIA::v2::Node2>::const_iterator requested_it)
{
    return Container<OSSIA::v2::Node2>::iterator();
}

# pragma mark -
# pragma mark Callback

OSSIA::v2::Node2::iterator BasicNode::addCallback(OSSIA::v2::NodeChangeCallback callback)
{
    return OSSIA::v2::Node2::iterator();
}

void BasicNode::removeCallback(Node2::iterator callback)
{
  CallbackContainer::removeCallback(callback);

  if (callbacks().size() == 0)
  {
    // stop children changes observation
    for (auto child : children())
    {
      Node2::iterator callbackIndex = mChildNodeChangeCallbackIndexes.find(child)->second;
      child->removeCallback(callbackIndex);
    }

    mChildNodeChangeCallbackIndexes.clear();
  }
}

}
