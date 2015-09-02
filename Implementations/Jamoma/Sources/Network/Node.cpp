#include "Network/JamomaNode.h"

#include <assert.h>

# pragma mark -
# pragma mark Life cycle

JamomaNode::JamomaNode(TTNodeDirectory * aDirectory, TTNode * aNode, shared_ptr<Device> aDevice, shared_ptr<JamomaNode> aParent) :
mDirectory(aDirectory),
mNode(aNode),
mDevice(aDevice),
mParent(aParent)
{}

JamomaNode::~JamomaNode()
{}

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

const shared_ptr<Address> & JamomaNode::getAddress() const
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
    TTObject object;
    
    // for local application case
    if (applicationType == kTTSym_local)
    {
      object = TTObject("Data", "parameter");
      object.set("rampDrive", kTTSym_none);
      mNode->setObject(object);
    }
    
    // for proxy application
    else if (applicationType == kTTSym_proxy)
    {
      TTAddress nodeAddress;
      mNode->getAddress(nodeAddress);
      TTValue args(nodeAddress, "parameter");
      object = getApplication().send("ProxyDataInstantiate", args);
      object.set("rampDrive", kTTSym_none);
    }
    
    // for mirror application
    else if (applicationType == kTTSym_mirror)
    {
      TTAddress nodeAddress;
      mNode->getAddress(nodeAddress);
      TTValue args(nodeAddress, "Data");
      object = getApplication().send("MirrorDataInstantiate", args);
      object.set("service", "parameter");
      
      //! \see in Device::create method, when creating Minuit protocol, some attributes are setup to be cached
      // all attributes handled by Address class should be in this list
    }
    
    if (object.valid())
    {
      if (type == Value::Type::IMPULSE)
        object.set("type", kTTSym_none);
      else if (type == Value::Type::BOOL)
        object.set("type", kTTSym_boolean);
      else if (type == Value::Type::INT)
        object.set("type", kTTSym_integer);
      else if (type == Value::Type::FLOAT)
        object.set("type", kTTSym_decimal);
      else if (type == Value::Type::CHAR)
        object.set("type", kTTSym_string);
      else if (type == Value::Type::STRING)
        object.set("type", kTTSym_string);
      else if (type == Value::Type::TUPLE)
        object.set("type", kTTSym_array);
      else if (type == Value::Type::GENERIC)
        object.set("type", kTTSym_generic);
    }
    
    // edit new address
    mAddress = shared_ptr<Address>(new JamomaAddress(shared_from_this(), object));
  }
  
  return mAddress;
}

bool JamomaNode::removeAddress()
{
  if (mNode)
  {
    mAddress = nullptr;
    return !mNode->setObject();
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
        mAddress = shared_ptr<Address>(new JamomaAddress(shared_from_this(), object));
      }
    }
  }
}