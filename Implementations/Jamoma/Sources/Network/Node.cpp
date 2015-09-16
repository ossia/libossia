#include "Network/JamomaNode.h"

#include <assert.h>

# pragma mark -
# pragma mark Life cycle

JamomaNode::JamomaNode(TTNodeDirectory * aDirectory, TTNode * aNode, shared_ptr<Device> aDevice, shared_ptr<JamomaNode> aParent) :
mDirectory(aDirectory),
mNode(aNode),
mObject("NodeInfo"),
mDevice(aDevice),
mParent(aParent)
{
    // if no object add a default NodeInfo object
    // otherwise TTNodeDirectory automatically removes
    // empty parent binding on no object when destroying the last child
    if (mNode->getObject() == nullptr)
      mNode->setObject(mObject);
}

JamomaNode::~JamomaNode()
{
  mAddress = nullptr;

  // if not a device node
  if (getParent() != nullptr)
  {
    TTAddress adrs;
    mNode->getAddress(adrs);
    mDirectory->TTNodeRemove(adrs);
  }
}

# pragma mark -
# pragma mark Navigation

shared_ptr<Device> JamomaNode::getDevice() const
{
  return mDevice.lock();
}

shared_ptr<Node> JamomaNode::getParent() const
{
  return mParent.lock();
}

# pragma mark -
# pragma mark Accessors

string JamomaNode::getName() const
{
  if (mNode)
  {
    TTString nameInstance = mNode->getName().c_str();
    if (mNode->getInstance() != kTTSymEmpty)
    {
      nameInstance += ".";
      nameInstance += mNode->getInstance().c_str();
    }

    return nameInstance.data();
  }

  return "";
}

Node & JamomaNode::setName(std::string name)
{
  TTAddress nameInstance(name.data());
  TTSymbol newInstance;
  TTBoolean newInstanceCreated;

  mNode->setNameInstance(nameInstance, newInstance, &newInstanceCreated);

  return *this;
}

shared_ptr<Address> JamomaNode::getAddress() const
{
  return mAddress;
}

# pragma mark -
# pragma mark Address

shared_ptr<Address> JamomaNode::createAddress(Value::Type type)
{
  // clear former address
  removeAddress();

  if (mNode)
  {
    TTSymbol applicationType = getApplicationType();

    // for local application case
    if (applicationType == kTTSym_local)
    {
      mObject = TTObject("Data", "parameter");
      mObject.set("rampDrive", kTTSym_none);
      mNode->setObject(mObject);
    }

    // for proxy application
    else if (applicationType == kTTSym_proxy)
    {
      TTAddress nodeAddress;
      mNode->getAddress(nodeAddress);
      TTValue args(nodeAddress, "parameter");
      mObject = getApplication().send("ProxyDataInstantiate", args);
      mObject.set("rampDrive", kTTSym_none);
    }

    // for mirror application
    else if (applicationType == kTTSym_mirror)
    {
      TTAddress nodeAddress;
      mNode->getAddress(nodeAddress);
      TTValue args(nodeAddress, "Data");
      mObject = getApplication().send("MirrorObjectInstantiate", args);
      mObject.set("service", "parameter");

      //! \see in Device::create method, when creating Minuit protocol, some attributes are setup to be cached
      // all attributes handled by Address class should be in this list
    }

    if (type == Value::Type::IMPULSE)
      mObject.set("type", kTTSym_none);
    else if (type == Value::Type::BOOL)
      mObject.set("type", kTTSym_boolean);
    else if (type == Value::Type::INT)
      mObject.set("type", kTTSym_integer);
    else if (type == Value::Type::FLOAT)
      mObject.set("type", kTTSym_decimal);
    else if (type == Value::Type::CHAR)
      mObject.set("type", kTTSym_string);
    else if (type == Value::Type::STRING)
      mObject.set("type", kTTSym_string);
    else if (type == Value::Type::TUPLE)
      mObject.set("type", kTTSym_array);
    else if (type == Value::Type::GENERIC)
      mObject.set("type", kTTSym_generic);

    // edit new address
    mAddress = make_shared<JamomaAddress>(shared_from_this(), mObject);
  }

  return mAddress;
}

bool JamomaNode::removeAddress()
{
  if (mNode)
  {
    mAddress = nullptr;

    // add a NodeInfo object otherwise TTNodeDirectory
    // automatically removes empty parent binding on no object
    // when destroying the last child
    mObject = TTObject("NodeInfo");
    return !mNode->setObject(mObject);
  }

  return false;
}

# pragma mark -
# pragma mark Children

Container<Node>::iterator JamomaNode::emplace(Container<Node>::const_iterator pos, string name)
{
  assert(!name.empty());

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
    return children().insert(pos, make_shared<JamomaNode>(mDirectory, node, mDevice.lock(), shared_from_this()));
  }

  return Container<Node>::iterator();
}

Container<Node>::iterator JamomaNode::insert(Container<Node>::const_iterator pos, shared_ptr<Node> node, std::string name)
{
  assert(!name.empty());

  //! \todo here the alias is not effective into the Jamoma tree so the given name is not used ...

  return children().insert(pos, node);
}

# pragma mark -
# pragma mark Implementation specific

TTObject & JamomaNode::getApplication() const
{
  return mDirectory->getRoot()->getObject();
}

TTSymbol JamomaNode::getApplicationType() const
{
  TTSymbol type;
  mDirectory->getRoot()->getObject().get("type", type);

  return type;
}

bool JamomaNode::updateChildren()
{
  //! \note this method is only available for root node for the moment
  if (mNode->getObject() != getApplication())
    return false;

  // tell our application to rebuild
  //! \todo rebuild from ourself
  TTErr err = getApplication().send("DirectoryBuild");

  // update root node
  this->mNode = this->mDirectory->getRoot();

  // erase all former nodes
  m_children.clear();

  // rebuild from tree
  buildChildren();

  // is there children below ?
  if (children().size() == 0)
    throw runtime_error("children empty after the update");

  return err == kTTErrNone;
}

void JamomaNode::buildChildren()
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
      shared_ptr<JamomaNode> newNode = make_shared<JamomaNode>(mDirectory, child, mDevice.lock(), shared_from_this());

      // build address if the node already manages a valid Data or Mirror object
      newNode->buildAddress();

      // store the child node
      children().push_back(newNode);

      // continue recursively
      newNode->buildChildren();
    }
  }
}

void JamomaNode::buildAddress()
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
      }
    }
  }
}
