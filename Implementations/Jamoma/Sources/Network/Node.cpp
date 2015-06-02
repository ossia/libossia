#include "Network/Node.h"
#include "Address.cpp"
#include "../Misc/Container.cpp"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaNode : public virtual Node, public enable_shared_from_this<JamomaNode>
{

protected:

  // Implementation specific
  TTNodeDirectory *       mDirectory{};
  TTNode *                mNode{};

  weak_ptr<JamomaNode>    mParent;
  shared_ptr<Address>     mAddress;
    
  string                  dName; // DEBUG

public:

  // Constructor, destructor
  JamomaNode(string name, TTNodeDirectory * aDirectory = nullptr, TTNode * aNode = nullptr, shared_ptr<JamomaNode> aParent = nullptr) :
  mDirectory(aDirectory),
  mNode(aNode),
  mParent(aParent)
  {
    if (mNode)
    {
      // auto edit address if the node already manages a valid Data or Mirror object
      TTObject object = mNode->getObject();
      if (object.valid())
      {
        TTSymbol objectName = object.name();

        if (objectName == kTTSym_Mirror)
          objectName = TTMirrorPtr(object.instance())->getName();

        if (objectName == "Data")
        {
          mAddress = shared_ptr<Address>(new JamomaAddress(object));
        }
      }
    }
    
    // DEBUG
    dName = name;
  }

  ~JamomaNode()
  {
    // DEBUG
    cout << "~JamomaNode() : " << dName << "\n";
  }

  // Navigation
  virtual Node & getParent() const override
  {
    shared_ptr<JamomaNode> p = mParent.lock();
    return *p;
  }

  // Accessors
  virtual string getName() const override
  {
    if (mNode)
      return mNode->getName().c_str();

    return "";
  }

  virtual const shared_ptr<Address> & getAddress() const override
  {
    return mAddress;
  }

  virtual bool removeAddress() override
  {
    if (mNode)
    {
      return !mNode->setObject();
    }

    return false;
  }

  // Address Factory
  virtual shared_ptr<Address> createAddress(AddressValue::Type type) override
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
        mNode->setObject(object);
      }

      // for proxy application
      else if (applicationType == kTTSym_proxy)
      {
        TTAddress nodeAddress;
        mNode->getAddress(nodeAddress);
        TTValue args(nodeAddress, "parameter");
        object = getApplication().send("ProxyDataInstantiate", args);
      }

      // for mirror application
      else if (applicationType == kTTSym_mirror)
      {
        ; // todo : allow to use TTApplication::appendMirrorObject method
      }

      if (object.valid())
      {
        if (type == AddressValue::Type::NONE)
          object.set("type", kTTSym_none);
        else if (type == AddressValue::Type::BOOL)
          object.set("type", kTTSym_boolean);
        else if (type == AddressValue::Type::INT)
          object.set("type", kTTSym_integer);
        else if (type == AddressValue::Type::FLOAT)
          object.set("type", kTTSym_decimal);
        else if (type == AddressValue::Type::CHAR)
          object.set("type", kTTSym_string);
        else if (type == AddressValue::Type::STRING)
          object.set("type", kTTSym_string);
        else if (type == AddressValue::Type::TUPLE)
          object.set("type", kTTSym_array);
        else if (type == AddressValue::Type::GENERIC)
          object.set("type", kTTSym_generic);
      }

      // edit new address
      mAddress = shared_ptr<Address>(new JamomaAddress(object));
    }

    return mAddress;
  }

  // Child Node Factory
  virtual iterator emplace(const_iterator pos, string name) override
  {
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
      return children().insert(pos, std::make_shared<JamomaNode>(name, mDirectory, node, shared_from_this()));
    }

    return iterator();
  }

private:

  TTObject & getApplication()
  {
    return mDirectory->getRoot()->getObject();
  }

  TTSymbol getApplicationType()
  {
    TTSymbol type;
    mDirectory->getRoot()->getObject().get("type", type);

    return type;
  }

protected:

  void buildChildren()
  {
    TTList childrenList;

    mNode->getChildren(S_WILDCARD, S_WILDCARD, childrenList);
    
    if (!childrenList.isEmpty())
    {
      // build a node for each child
      for (childrenList.begin(); childrenList.end(); childrenList.next())
      {
        TTNodePtr child = TTNodePtr(TTPtr(childrenList.current()[0]));
        
        // build child name
        TTString nameInstance = child->getName().c_str();
        if (child->getInstance() != kTTSymEmpty)
        {
          nameInstance += child->getInstance().c_str();
        }
        
        // build child node
        shared_ptr<JamomaNode> newNode = std::make_shared<JamomaNode>(nameInstance.data(), mDirectory, child, shared_from_this());
        
        // store the child node
        children().push_back(newNode);
        
        // continue recursively
        newNode->buildChildren();
      }
    }
  }
};
