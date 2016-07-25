#include "BasicNode.h"
#include "BasicDevice.h"
#include <Editor/Value/Value.h>
#include <assert.h>

namespace impl
{
BasicNode::BasicNode(shared_ptr<Device> aDevice, shared_ptr<BasicNode> aParent) :
  mDevice(aDevice),
  mParent(aParent)
{
}

BasicNode::~BasicNode()
{
  m_children.clear();
}


shared_ptr<Device> BasicNode::getDevice() const
{
  return mDevice.lock();
}

shared_ptr<Node> BasicNode::getParent() const
{
  return mParent.lock();
}

shared_ptr<Node> BasicNode::getThis()
{
  return shared_from_this();
}

std::string BasicNode::getName() const
{
  return mName;
}

Node & BasicNode::setName(std::string name)
{
  mName = std::move(name);
  // notify observers
  send(*this, oldName, NodeChange::RENAMED);

  return *this;
}

shared_ptr<Address> BasicNode::getAddress() const
{
  return mAddress;
}

shared_ptr<Address> BasicNode::createAddress(Type type)
{
  // clear former address
  removeAddress();

  // edit new address
  mAddress = make_shared<impl::BasicAddress>(shared_from_this());

  // set type
  mAddress->setValueType(type);

  // notify observers
  send(*this, getName(), NodeChange::ADDRESS_CREATED);

  return mAddress;
}

bool BasicNode::removeAddress()
{
  if (mNode)
  {
    // use the device protocol to stop address value observation
    if (mAddress)
    {
      if (auto device = getDevice())
      {
        device->getProtocol()->observeAddressValue(mAddress, false);
      }

      mAddress.reset();
    }

    // add a NodeInfo object otherwise TTNodeDirectory
    // automatically removes empty parent binding on no object
    // when destroying the last child
    mObject = TTObject("NodeInfo");
    TTErr err = mNode->setObject(mObject);

    // notify observers
    send(*this, getName(), NodeChange::ADDRESS_REMOVED);

    return !err;
  }

  return false;
}

# pragma mark -
# pragma mark Children

Container<Node>::iterator BasicNode::emplace(Container<Node>::const_iterator pos, string name)
{
  if (name.empty())
    throw runtime_error("can't create a node with empty name");

  TTAddress nodeAddress;
  mNode->getAddress(nodeAddress);

  TTAddress   address = nodeAddress.appendAddress(TTAddress(name.data()));
  TTObject    empty;
  TTPtr       context = NULL;
  TTNodePtr   node;
  TTBoolean   newInstanceCreated;

  TTErr err = mDirectory->TTNodeCreate(address, empty, context, &node, &newInstanceCreated);

  if (!err)
  {
    // store the new node into the Container
    auto newNode = make_shared<BasicNode>(mDirectory, node, mDevice.lock(), shared_from_this());
    Container<Node>::iterator it = m_children.insert(pos, newNode);

    // notify observers
    send(*newNode, newNode->getName(), NodeChange::EMPLACED);

    // start child changes observation if needed
    if (callbacks().size() >= 1)
    {
      mChildNodeChangeCallbackIndexes.emplace(
            std::make_pair(
              newNode,
              newNode->addCallback([=] (const Node& child, const std::string& name, NodeChange change) {
        return childNodeChangeCallback(child, name, change);
      }
      )));
    }

    return it;
  }

  return Container<Node>::iterator();
}

Container<Node>::iterator BasicNode::emplace(Container<Node>::const_iterator pos,
                                              const string& name,
                                              Type type,
                                              AccessMode access,
                                              const std::shared_ptr<Domain>& domain,
                                              BoundingMode bm,
                                              bool repetitionFilter)
{
  if (name.empty())
    throw runtime_error("can't create a node with empty name");

  TTAddress nodeAddress;
  mNode->getAddress(nodeAddress);

  TTAddress   address = nodeAddress.appendAddress(TTAddress(name.data()));
  TTObject    empty;
  TTPtr       context = NULL;
  TTNodePtr   node;
  TTBoolean   newInstanceCreated;

  TTErr err = mDirectory->TTNodeCreate(address, empty, context, &node, &newInstanceCreated);

  if (!err)
  {
    // create a new node and its address
    auto newNode = make_shared<BasicNode>(mDirectory, node, mDevice.lock(), shared_from_this());
    std::shared_ptr<OSSIA::Address> addr = newNode->createAddress(type);

    addr->setBoundingMode(bm);
    addr->setAccessMode(access);
    if(domain)
      addr->setDomain(domain);
    addr->setBoundingMode(bm);
    addr->setRepetitionFilter(repetitionFilter);

    // store the new node into the Container
    Container<Node>::iterator it = m_children.insert(pos, newNode);

    // notify observers
    send(**it, getName(), NodeChange::EMPLACED);

    // start child changes observation if needed
    if (callbacks().size() >= 1)
    {
      mChildNodeChangeCallbackIndexes.emplace(
            std::make_pair(
              newNode,
              newNode->addCallback(
                [=] (const Node& child, const std::string& name, NodeChange change) {
        return childNodeChangeCallback(child, name, change);
      })));
    }

    return it;
  }

  return Container<Node>::iterator();
}

Container<Node>::iterator BasicNode::insert(Container<Node>::const_iterator pos, shared_ptr<Node> node, std::string name)
{
  assert(!name.empty());

  //! \todo here the alias is not effective into the Jamoma tree so the given name is not used ...

  return m_children.insert(pos, node);
}

Container<Node>::iterator BasicNode::erase(Container<Node>::const_iterator requested_it)
{
  // \todo shouldn't we remove the parent here, in case another class keeps a shared_ptr ?
  // (which would cause the node to still be alive)

  // remove the all addresses below this node
  auto child = *requested_it;
  auto& jnode = dynamic_cast<BasicNode&>(*child);
  jnode.removeAddresses();

  // remove the node
  Container<Node>::iterator it = m_children.erase(requested_it);

  // notify observers
  send(*child, child->getName(), NodeChange::ERASED);

  // stop child observation if needed
  if (callbacks().size() >= 1)
  {
    Node::iterator callbackIndex = mChildNodeChangeCallbackIndexes.find(child)->second;
    child->removeCallback(callbackIndex);
  }

  return it;
}

# pragma mark -
# pragma mark Callback

Node::iterator BasicNode::addCallback(NodeChangeCallback callback)
{
  auto it = CallbackContainer::addCallback(std::move(callback));

  if (callbacks().size() == 1)
  {
    // start children changes observation
    for (auto child : children())
    {
      mChildNodeChangeCallbackIndexes.emplace(
            std::make_pair(
              child,
              child->addCallback(
                [=] (const Node& child, const std::string& name, NodeChange change) {
        return childNodeChangeCallback(child, name, change);
      })));
    }
  }

  return it;
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

# pragma mark -
# pragma mark Implementation specific

TTObject & BasicNode::getApplication() const
{
  return mDirectory->getRoot()->getObject();
}

TTSymbol BasicNode::getApplicationType() const
{
  TTSymbol type;
  mDirectory->getRoot()->getObject().get("type", type);

  return type;
}

bool BasicNode::updateChildren()
{
  //! \note this method is only available for root node for the moment
  if (mNode->getObject() != getApplication())
    return false;

  // erase all former nodes
  m_children.clear();

  // tell our application to rebuild
  //! \todo rebuild from ourself
  TTErr err = getApplication().send("DirectoryBuild");

  // update root node
  this->mNode = this->mDirectory->getRoot();

  // rebuild from tree
  buildChildren();

  // is there children below ?
  if (children().size() == 0)
    throw runtime_error("children empty after the update");

  return err == kTTErrNone;
}

void BasicNode::buildChildren()
{
  TTList childrenList;

  mNode->getChildren(S_WILDCARD, S_WILDCARD, childrenList);

  if (!childrenList.isEmpty())
  {
    // sort children by priority
    childrenList.sort(&compareNodePriorityThenNameThenInstance);

    // build a node for each child
    for (childrenList.begin(); childrenList.end(); childrenList.next())
    {
      TTNodePtr child = TTNodePtr(TTPtr(childrenList.current()[0]));

      // build child node
      shared_ptr<BasicNode> newNode = make_shared<BasicNode>(mDirectory, child, mDevice.lock(), shared_from_this());

      // build address if the node already manages a valid Data or Mirror object
      newNode->buildAddress();

      // store the child node
      m_children.push_back(newNode);

      // continue recursively
      newNode->buildChildren();
    }
  }
}

void BasicNode::buildAddress()
{
  if (mNode)
  {
    TTObject object = mNode->getObject();
    if (object.valid())
    {
      TTSymbol objectName = object.name();

      if (objectName == kTTSym_Mirror)
        objectName = TTMirrorPtr(object.instance())->getName();

      if (objectName == "Data")
      {
        mAddress = make_shared<JamomaAddress>(shared_from_this(), object);

        // edit value type, access mode, bounding mode and repetition filter attribute
        TTSymbol type;
        object.get("type", type);

        if (type == kTTSym_none)
        {
          mAddress->setValue(Impulse{});
          mAddress->setValueType(Type::IMPULSE);
        }
        else if (type == kTTSym_generic)
        {
          mAddress->setValue(Tuple{});
          mAddress->setValueType(Type::TUPLE);
        }
        else if (type == kTTSym_boolean)
        {
          mAddress->setValue(Bool{});
          mAddress->setValueType(Type::BOOL);
        }
        else if (type == kTTSym_integer)
        {
          mAddress->setValue(Int{});
          mAddress->setValueType(Type::INT);
        }
        else if (type == kTTSym_decimal)
        {
          mAddress->setValue(Float{});
          mAddress->setValueType(Type::FLOAT);
        }
        else if (type == kTTSym_array)
        {
          mAddress->setValue(Tuple{});
          mAddress->setValueType(Type::TUPLE);
        }
        else if (type == kTTSym_string)
        {
          mAddress->setValue(String{});
          mAddress->setValueType(Type::STRING);
        }

        TTSymbol service;
        object.get("service", service);

        if (service == kTTSym_parameter)
          mAddress->setAccessMode(OSSIA::AccessMode::BI);
        else if (service == kTTSym_message)
          mAddress->setAccessMode(OSSIA::AccessMode::SET);
        else if (service == kTTSym_return)
          mAddress->setAccessMode(OSSIA::AccessMode::GET);

        TTValue range;
        object.get("rangeBounds", range);

        if (type == kTTSym_none)
        {
          mAddress->setDomain(Domain::create());
        }
        else if (type == kTTSym_generic)
        {
          mAddress->setDomain(Domain::create());
        }
        else if (type == kTTSym_boolean)
        {
          if (range.size() == 2)
          {
            mAddress->setDomain(Domain::create(OSSIA::Bool{range[0]}, OSSIA::Bool{range[1]}));
          }
        }
        else if (type == kTTSym_integer)
        {
          if (range.size() == 2)
          {
            mAddress->setDomain(Domain::create(OSSIA::Int{range[0]}, OSSIA::Int{range[1]}));
          }
        }
        else if (type == kTTSym_decimal)
        {
          if (range.size() == 2)
          {
            mAddress->setDomain(Domain::create(OSSIA::Float{range[0]}, OSSIA::Float{range[1]}));
          }
        }
        else if (type == kTTSym_array)
        {
          // we need to know the size of the array to setup the domain
          TTValue v;
          object.get("value", v);

          vector<Value> tuple_min;
          vector<Value> tuple_max;
          for (unsigned long i = 0; i < v.size(); i++)
            tuple_min.push_back(OSSIA::Float{range[0]});

          tuple_max.push_back(OSSIA::Float{range[1]});

          mAddress->setDomain(Domain::create(OSSIA::Tuple{std::move(tuple_min)}, OSSIA::Tuple{std::move(tuple_max)}));
        }
        else if (type == kTTSym_string)
        {
          // string values enumeration
          vector<Value> values;
          for (const auto & e : range)
          {
            TTSymbol s = e;
            values.push_back(OSSIA::String(s.c_str()));
          }
          mAddress->setDomain(Domain::create(OSSIA::Impulse{}, OSSIA::Impulse{}, values));
        }
        else
        {
          mAddress->setDomain(Domain::create());
        }

        TTSymbol clipmode;
        object.get("rangeClipmode", clipmode);

        if (clipmode == kTTSym_none)
          mAddress->setBoundingMode(OSSIA::BoundingMode::FREE);
        else if (clipmode == kTTSym_low)
          mAddress->setBoundingMode(OSSIA::BoundingMode::CLIP);
        else if (clipmode == kTTSym_high)
          mAddress->setBoundingMode(OSSIA::BoundingMode::CLIP);
        else if (clipmode == kTTSym_both)
          mAddress->setBoundingMode(OSSIA::BoundingMode::CLIP);
        else if (clipmode == kTTSym_wrap)
          mAddress->setBoundingMode(OSSIA::BoundingMode::WRAP);
        else if (clipmode == kTTSym_fold)
          mAddress->setBoundingMode(OSSIA::BoundingMode::FOLD);

        TTBoolean repetitionFilter;
        object.get("repetitionFilter", repetitionFilter);

        mAddress->setRepetitionFilter(repetitionFilter);
      }
    }
  }
}

void BasicNode::childNodeChangeCallback(const Node& child, const std::string& name, NodeChange change)
{
  // only notify tree structure changes to parent
  if (change == NodeChange::EMPLACED || change == NodeChange::ERASED || change == NodeChange::RENAMED)
    send(child, name, change);
}

void BasicNode::removeAddresses()
{
  if(mAddress)
  {
    // close value listening
    getDevice()->getProtocol()->observeAddressValue(mAddress, false);

    // reset the shared_ptr
    mAddress.reset();
  }

  // do the same for all children
  for(auto& child : children())
  {
    auto& jnode = dynamic_cast<BasicNode&>(*child);
    jnode.removeAddresses();
  }
}
}
