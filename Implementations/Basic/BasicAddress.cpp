#include "BasicAddress.h"
#include <Editor/Value/Value.h>
#include "Network/NetworkLogger.h"

#include <map>
#include <iostream> //! \todo to remove. only here for debug purpose

namespace impl {

BasicAddress::BasicAddress(weak_ptr<Node> node) :
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
  mTextualAddress = getAddressFromNode(*mNode.lock());
}

BasicAddress::~BasicAddress()
{
}

const shared_ptr<Node> BasicAddress::getNode() const
{
  return mNode.lock();
}

void BasicAddress::pullValue()
{
  // use the device protocol to pull address value
  getProtocol().pullAddressValue(*this);
}

OSSIA::Address2& BasicAddress::pushValue(const Value& value)
{
  setValue(value);

  // use the device protocol to push address value
  getProtocol().pushAddressValue(*this);

  return *this;
}

OSSIA::Address2& BasicAddress::pushValue()
{
  // use the device protocol to push address value
  getProtocol().pushAddressValue(*this);

  return *this;
}

const OSSIA::Value& BasicAddress::getValue() const
{
  return mValue;
}

OSSIA::Value BasicAddress::cloneValue(std::vector<char> index) const
{
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

OSSIA::Address2& BasicAddress::setValue(const Value& value)
{
  std::lock_guard<std::mutex> lock(mValueMutex);

  // set value querying the value from another address
  auto dest = value.try_get<Destination>();
  if (dest && mValueType != Type::DESTINATION)
  {
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

OSSIA::Address2& BasicAddress::setValueType(Type type)
{
  mValueType = type;

  mValue = initValue(type);

  return *this;
}

AccessMode BasicAddress::getAccessMode() const
{
  return mAccessMode;
}

OSSIA::Address2& BasicAddress::setAccessMode(AccessMode accessMode)
{
  mAccessMode = accessMode;
  return *this;
}

const std::shared_ptr<OSSIA::Domain> & BasicAddress::getDomain() const
{
  return mDomain;
}

OSSIA::Address2& BasicAddress::setDomain(std::shared_ptr<OSSIA::Domain> domain)
{
  mDomain = domain;
  return *this;
}

BoundingMode BasicAddress::getBoundingMode() const
{
  return mBoundingMode;
}

OSSIA::Address2& BasicAddress::setBoundingMode(OSSIA::BoundingMode boundingMode)
{
  mBoundingMode = boundingMode;
  return *this;
}

bool BasicAddress::getRepetitionFilter() const
{
  return mRepetitionFilter;
}

OSSIA::Address2& BasicAddress::setRepetitionFilter(bool repetitionFilter)
{
  mRepetitionFilter = repetitionFilter;

  return *this;
}

# pragma mark -
# pragma mark Callback

OSSIA::Address2::iterator BasicAddress::addCallback(OSSIA::ValueCallback callback)
{
  auto it = CallbackContainer::addCallback(std::move(callback));

  if (callbacks().size() == 1)
  {
    // use the device protocol to start address value observation
    getProtocol().observeAddressValue(shared_from_this(), true);

    //! \debug
    //cout << "opening listening on " << buildNodePath(mNode.lock()) << endl;
  }

  return it;
}

void BasicAddress::removeCallback(Address2::iterator callback)
{
  CallbackContainer::removeCallback(callback);

  if (callbacks().size() == 0)
  {
    // use the device protocol to stop address value observation
    getProtocol().observeAddressValue(shared_from_this(), false);

    //! \debug
    //cout << "closing listening on " << buildNodePath(mNode.lock()) << endl;
  }
}

OSSIA::Value initValue(OSSIA::Type type)
{
  switch(type)
  {
  case Type::IMPULSE:
    return Impulse{};
  case Type::BOOL:
    return Bool{};
  case Type::INT:
    return Int{};
  case Type::FLOAT:
    return Float{};
  case Type::CHAR:
    return Char{};
  case Type::STRING:
    return String{};
  case Type::TUPLE:
    return Tuple{};
  case Type::VEC2F:
    return Vec2f{};
  case Type::VEC3F:
    return Vec3f{};
  case Type::VEC4F:
    return Vec4f{};
  case Type::DESTINATION:
    return Destination{};
  case Type::BEHAVIOR:
    return Behavior{{}};
  }

  throw std::runtime_error("Invalid type");
}

std::string BasicAddress::buildNodePath(std::shared_ptr<OSSIA::Node> node)
{
  std::string path;
  std::string name = node->getName();
  std::shared_ptr<OSSIA::Node> parent = node->getParent();

  if (parent != nullptr)
  {
    path += buildNodePath(parent);
    if (path != "/")
      path += "/";
    path += name;
  }
  else
  {
    //! \todo use device name
    path = name;
  }

  return path;
}

OSSIA::Protocol2& BasicAddress::getProtocol() const
{
  if(auto cached_proto = mProtocolCache.lock())
  {
    return *cached_proto.get();
  }
  else
  {
    auto node = mNode.lock();
    if(!node)
      return getDummyProtocol();
    auto dev = node->getDevice();
    if(!dev)
      return getDummyProtocol();
    auto proto = dev->getProtocol();
    if(!proto)
      return getDummyProtocol();

    // Save in the cache
    mProtocolCache = proto;
    return *proto.get();
  }
}

OSSIA::Protocol& getDummyProtocol()
{
  struct DummyProtocol : public OSSIA::Protocol2
  {
    bool pullAddressValue(OSSIA::Address2&) const override { return true; }
    bool pushAddressValue(const OSSIA::Address2&) const override { return true; }
    bool observeAddressValue(std::shared_ptr<OSSIA::Address2>, bool) const override { return false; }
    bool updateChildren(OSSIA::Node2&) const override { return false; }
    void setLogger(std::shared_ptr<OSSIA::NetworkLogger>) override { }
    std::shared_ptr<OSSIA::NetworkLogger> getLogger() const override { return{}; }
  };

  static DummyProtocol proto;
  return proto;
}
}
