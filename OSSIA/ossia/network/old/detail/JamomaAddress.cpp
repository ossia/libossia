#include <ossia/network/v1/detail/JamomaAddress.hpp>
#include <ossia/network/v1/Domain.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/network/common/network_logger.hpp>

#include <map>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

static auto& symbol_map()
{
  static const std::map<OSSIA::Type, TTSymbol>& symmap{
    { Type::IMPULSE, kTTSym_none },
    { Type::BOOL, kTTSym_boolean },
    { Type::INT, kTTSym_integer },
    { Type::FLOAT, kTTSym_decimal },
    { Type::CHAR, kTTSym_string },
    { Type::STRING, kTTSym_string },
    { Type::TUPLE, kTTSym_array },
    { Type::VEC2F, kTTSym_array },
    { Type::VEC3F, kTTSym_array },
    { Type::VEC4F, kTTSym_array },
    { Type::DESTINATION, kTTSym_string },
  };

  return symmap;
}

JamomaAddress::JamomaAddress(weak_ptr<Node> node, TTObject aData) :
mNode(node),
mObject(aData),
mObjectValueCallback("callback"),
mValue(Impulse{}),
mValueType(Type::IMPULSE),
mAccessMode(AccessMode::BI),
mBoundingMode(BoundingMode::FREE),
mRepetitionFilter(false)
{
  if (mObject.valid())
  {
    // prepare callback for value observation
    TTValue args(TTPtr(this), mObject);
    mObjectValueCallback.set("baton", args);
    mObjectValueCallback.set("function", TTPtr(&JamomaAddress::TTValueCallback));
  }

  // To set-up the protocol cache.
  // Note : the cache works because the nodes cannot change parents.
  getProtocol();
  mTextualAddress = getAddressFromNode(*mNode.lock());
}

JamomaAddress::~JamomaAddress()
{
}

# pragma mark -
# pragma mark Network

const std::shared_ptr<Node> JamomaAddress::getNode() const
{
  return mNode.lock();
}

void JamomaAddress::pullValue()
{
  // use the device protocol to pull address value
  getProtocol().pullAddressValue(*this);
}

Address & JamomaAddress::pushValue(const Value& value)
{
  setValue(value);

  // use the device protocol to push address value
  getProtocol().pushAddressValue(*this);

  return *this;
}

Address & JamomaAddress::pushValue()
{
  // use the device protocol to push address value
  getProtocol().pushAddressValue(*this);

  return *this;
}

# pragma mark -
# pragma mark Accessors

const Value& JamomaAddress::getValue() const
{
  return mValue;
}

Value JamomaAddress::cloneValue(DestinationIndex index) const
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
      throw std::runtime_error("cloning null value");
  }
}

Address & JamomaAddress::setValue(const Value& value)
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
        throw std::runtime_error("setting an address value using a destination with a bad type address");
    }
    else
    {
      throw std::runtime_error("setting an address value using a destination without address");
    }
  }

  // copy the new value
  else
  {
    if(mValue.v.which() != value.v.which())
    {
      mValueType = value.getType();

      if (mObject.name() != kTTSym_Mirror)
      {
        auto& symmap = symbol_map();
        auto it = symmap.find(mValueType);
        if(it != symmap.end())
        {
          mObject.set("type", it->second);
        }
      }
    }

    mValue = value;
  }

  return *this;
}

Type JamomaAddress::getValueType() const
{
  return mValueType;
}

Address & JamomaAddress::setValueType(Type type)
{
  mValueType = type;

  if (mObject.name() != kTTSym_Mirror)
  {
    auto& symmap = symbol_map();
    auto it = symmap.find(mValueType);
    if(it != symmap.end())
    {
      mObject.set("type", it->second);
    }
  }

  // initialize the value member
  initValue();

  return *this;
}

AccessMode JamomaAddress::getAccessMode() const
{
  return mAccessMode;
}

Address & JamomaAddress::setAccessMode(AccessMode accessMode)
{
  mAccessMode = accessMode;

  if (mObject.name() != kTTSym_Mirror)
  {
    if (mAccessMode == AccessMode::BI)
      mObject.set("service", kTTSym_parameter);
    else if (mAccessMode == AccessMode::SET)
      mObject.set("service", kTTSym_message);
    else if (mAccessMode == AccessMode::GET)
      mObject.set("service", kTTSym_return);
  }

  return *this;
}

const std::shared_ptr<Domain> & JamomaAddress::getDomain() const
{
  return mDomain;
}

Address & JamomaAddress::setDomain(std::shared_ptr<Domain> domain)
{
  mDomain = domain;

  TTValue range, v;

  if(mDomain)
  {
      if (mDomain->getValues().empty())
      {
          convertValueIntoTTValue(mDomain->getMin(), v);
          range.append(v);

          convertValueIntoTTValue(mDomain->getMax(), v);
          range.append(v);
      }
      else
      {
          for (const auto & e : mDomain->getValues())
          {
              convertValueIntoTTValue(e, v);
              range.append(v);
          }
      }
  }

  if (mObject.name() != kTTSym_Mirror)
  {
    mObject.set("rangeBounds", range);
  }

  return *this;
}

BoundingMode JamomaAddress::getBoundingMode() const
{
  return mBoundingMode;
}

Address & JamomaAddress::setBoundingMode(BoundingMode boundingMode)
{
  mBoundingMode = boundingMode;

  if (mObject.name() != kTTSym_Mirror)
  {
    if (mBoundingMode == BoundingMode::FREE)
      mObject.set("rangeClipmode", kTTSym_none);
    else if (mBoundingMode == BoundingMode::CLIP)
      mObject.set("rangeClipmode", kTTSym_both);
    else if (mBoundingMode == BoundingMode::WRAP)
      mObject.set("rangeClipmode", kTTSym_wrap);
    else if (mBoundingMode == BoundingMode::FOLD)
      mObject.set("rangeClipmode", kTTSym_fold);
  }

  return *this;
}

bool JamomaAddress::getRepetitionFilter() const
{
  return mRepetitionFilter;
}

Address & JamomaAddress::setRepetitionFilter(bool repetitionFilter)
{
  mRepetitionFilter = repetitionFilter;

  if (mObject.name() != kTTSym_Mirror)
  {
    mObject.set("repetitionsFilter", repetitionFilter);
  }

  return *this;
}

# pragma mark -
# pragma mark Callback

Address::iterator JamomaAddress::addCallback(ValueCallback callback)
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

void JamomaAddress::removeCallback(Address::iterator callback)
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

# pragma mark -
# pragma mark Implementation specific

TTErr JamomaAddress::TTValueCallback(const TTValue& baton, const TTValue& value)
{
  JamomaAddress * self = static_cast<JamomaAddress*>(TTPtr(baton[0]));
  TTObject aData = baton[1];

  // check data object
  if (aData.instance() == self->mObject.instance())
  {
    self->setValue(value);

    // We clone the value to prevent crashes
    // if it is rewritten afterwards.
    try {
        auto val = self->cloneValue();
        if(auto logger = self->getProtocol().getLogger())
        {
            if(auto& log_callback = logger->getInboundLogCallback())
                log_callback(self->getTextualAddress() + " <= " + getValueAsString(self->getValue()));
        }
        self->send(val);
    }
    catch(...) {
        return kTTErrGeneric;
    }

    return kTTErrNone;
  }

  return kTTErrGeneric;
}

void JamomaAddress::initValue()
{
  switch(mValueType)
  {
      case Type::IMPULSE:
          mValue = Impulse{};
          break;
      case Type::BOOL:
          mValue = Bool{};
          break;
      case Type::INT:
          mValue = Int{};
          break;
      case Type::FLOAT:
          mValue = Float{};
          break;
      case Type::CHAR:
          mValue = Char{};
          break;
      case Type::STRING:
          mValue = String{};
          break;
      case Type::TUPLE:
          mValue = Tuple{};
          break;
      case Type::VEC2F:
          mValue = Vec2f{};
          break;
      case Type::VEC3F:
          mValue = Vec3f{};
          break;
      case Type::VEC4F:
          mValue = Vec4f{};
          break;
      case Type::DESTINATION:
          mValue = Destination{};
          break;
      case Type::BEHAVIOR:
          return;
  }
}

bool JamomaAddress::pullValue(TTValue& value) const
{
  return !mObject.get("value", value);
}

bool JamomaAddress::pushValue(const TTValue& value) const
{
  TTErr err;

  if (mObject.name() == "Data")
    err = mObject.send("Command", value);
  else
    err = mObject.set("value", value);

  return !err;
}

void JamomaAddress::getValue(TTValue& value) const
{
  std::lock_guard<std::mutex> lock(mValueMutex);

  // convert current value
  convertValueIntoTTValue(mValue, value);
}

void JamomaAddress::setValue(const TTValue& value)
{
  std::lock_guard<std::mutex> lock(mValueMutex);

  // store new value
  mValue = convertTTValueIntoValue(value, mValueType);
}

void JamomaAddress::observeValue(bool enable)
{
  TTAttributePtr attribute;
  mObject.instance()->findAttribute("value", &attribute);

  if (enable)
  {
    attribute->registerObserverForNotifications(mObjectValueCallback);
  }
  else
  {
    attribute->unregisterObserverForNotifications(mObjectValueCallback);
  }

  // for Mirror object : enable listening
  if (mObject.name() == kTTSym_Mirror)
  {
    TTMirrorPtr(mObject.instance())->enableListening(*attribute, enable);
  }
}

Value JamomaAddress::convertTTValueIntoValue(const TTValue& v, Type valueType) const
{
  switch (valueType)
  {
    case Type::IMPULSE :
    {
      return OSSIA::Impulse{};
    }

    case Type::BOOL :
    {
      if (v.size() == 1)
        return OSSIA::Bool{v[0]};

      return OSSIA::Bool{};
    }

    case Type::INT :
    {
      if (v.size() == 1)
        return OSSIA::Int{v[0]};

      return OSSIA::Int{};
    }

    case Type::FLOAT :
    {
      if (v.size() == 1)
        return OSSIA::Float{v[0]};

      return OSSIA::Float{};
    }

    case Type::CHAR :
    {
      if (v.size() == 1)
      {
        if (v[0].type() == kTypeString)
        {
          char* c_value = TTString(v[0]).data();
          return OSSIA::Char{c_value[0]};
        }
      }

      return OSSIA::Char{};
    }

    case Type::STRING :
    {
      if (v.size() == 1)
      {
        if (v[0].type() == kTypeSymbol)
        {
          TTSymbol s_value = v[0];
          return OSSIA::String{s_value.c_str()};
        }
      }

      return OSSIA::String{};
    }

    case Type::DESTINATION :
    {
      throw std::runtime_error("convertion to destination value is not handled");
      /*
       if (v.size() == 1)
       {
       if (v[0].type() == kTypeSymbol)
       {
       //! \todo retreive the Address from the symbol
       //TTAddress s_value = v[0];

       return new OSSIA::Destination();
       }
       }

       return new OSSIA::Destination();
       */
    }

    case Type::BEHAVIOR :
    {
      break;
    }

    case Type::VEC2F :
    {
      const constexpr int n = 2;
      Vec<float, n> vec;

      if(v.size() != n)
        return vec;

      std::array<TTValue, n> vals{v[0], v[1]};
      std::array<TTDataType, n> types{v[0].type(), v[1].type()};
      if(std::any_of(types.begin(), types.end(),
                     [] (auto t) { return t != kTypeFloat32 || t != kTypeFloat64; }))
        return vec;

      for(int i = 0; i < n; i++)
        vec.value[i] = vals[i];

      return vec;
    }

    case Type::VEC3F :
    {
      const constexpr int n = 3;
      Vec<float, n> vec;

      if(v.size() != n)
        return vec;

      std::array<TTValue, n> vals{v[0], v[1], v[2]};
      std::array<TTDataType, n> types{v[0].type(), v[1].type(), v[2].type()};
      if(std::any_of(types.begin(), types.end(),
                     [] (auto t) { return t != kTypeFloat32 || t != kTypeFloat64; }))
        return vec;

      for(int i = 0; i < n; i++)
        vec.value[i] = vals[i];

      return vec;
    }

    case Type::VEC4F :
    {
      const constexpr int n = 4;
      Vec<float, n> vec;

      if(v.size() != n)
        return vec;

      std::array<TTValue, n> vals{v[0], v[1], v[2], v[3]};
      std::array<TTDataType, n> types{v[0].type(), v[1].type(), v[2].type(), v[3].type()};
      if(std::any_of(types.begin(), types.end(),
                     [] (auto t) { return t != kTypeFloat32 || t != kTypeFloat64; }))
        return vec;

      for(int i = 0; i < n; i++)
        vec.value[i] = vals[i];

      return vec;
    }


    case Type::TUPLE :
    {
      vector<Value> t_value;

      for (const auto & e : v)
      {
        Type type;

        if (e.type() == kTypeBoolean)
        {
          type = Type::BOOL;
        }
        else if (e.type() == kTypeInt8 || e.type() == kTypeUInt8 ||
                 e.type() == kTypeInt16 || e.type() == kTypeUInt16 ||
                 e.type() == kTypeInt32 || e.type() == kTypeUInt32 ||
                 e.type() == kTypeInt64 || e.type() == kTypeUInt64)
        {
          type = Type::INT;
        }
        else if (e.type() == kTypeFloat32 || e.type() == kTypeFloat64)
        {
          type = Type::FLOAT;
        }
        else if (e.type() == kTypeSymbol)
        {
          type = Type::STRING;
        }
        else
        {
          continue;
        }

        TTValue t(e);
        t_value.push_back(convertTTValueIntoValue(t, type));
      }

      return OSSIA::Tuple(std::move(t_value));
    }
  }

  return {};
}

void JamomaAddress::convertValueIntoTTValue(const Value& value, TTValue & ttval)
{
  struct visitor {
    TTValue& v;

    void operator()(Impulse) const { }
    void operator()(Int i) const   { v = TTInt32(i.value); }
    void operator()(Float f) const { v = TTFloat64(f.value); }
    void operator()(Bool b) const  { v = TTBoolean(b.value); }
    void operator()(Char c) const  { v = TTSymbol(c.value); }
    void operator()(const String& s) const { v = TTSymbol(s.value); }
    void operator()(Vec2f vec) const { v = TTValue{vec.value[0], vec.value[1]}; }
    void operator()(Vec3f vec) const { v = TTValue{vec.value[0], vec.value[1], vec.value[2]}; }
    void operator()(Vec4f vec) const { v = TTValue{vec.value[0], vec.value[1], vec.value[2], vec.value[3]}; }
    void operator()(const Destination& d) const { v = TTAddress(buildNodePath(d.value).data()); }
    void operator()(const Behavior&) const { }
    void operator()(const Tuple& t) const
    {
      for (const auto & e : t.value)
      {
        TTValue n;
        convertValueIntoTTValue(e, n);

        if (n.size())
          v.append(n[0]);
      }
    }

  } vis{ttval};

  if(value.valid())
    eggs::variants::apply(vis, value.v);
}

string JamomaAddress::buildNodePath(Node* node)
{
  string path;
  string name = node->getName();
  std::shared_ptr<Node> parent = node->getParent();

  if (parent != nullptr)
  {
    path += buildNodePath(parent.get());
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

Protocol& JamomaAddress::getProtocol() const
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

Protocol& getDummyProtocol()
{
    struct DummyProtocol : public Protocol
    {
            bool pullAddressValue(Address&) const override { return true; }
            bool pushAddressValue(const Address&) const override { return true; }
            bool observeAddressValue(std::shared_ptr<Address>, bool) const override { return false; }
            bool updateChildren(Node&) const override { return false; }
            void setLogger(std::shared_ptr<NetworkLogger>) override { }
            std::shared_ptr<NetworkLogger> getLogger() const override { return{}; }
    };

    static DummyProtocol proto;
    return proto;
}
