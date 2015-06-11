#include "Network/Address.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaAddress : public Address
{

private:

  // Implementation specific
  mutable TTObject            mData;
  mutable AddressValue *      mValue{};
  AddressValue::Type          mValueType;
  AccessMode                  mAccessMode;
  AddressDomain *             mDomain;
  BoundingMode                mBoundingMode;
  bool                        mRepetitionFilter;

  shared_ptr<Device>          device;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaAddress(TTObject aData = TTObject()) :
  mData(aData),
  mValueType(AddressValue::Type::IMPULSE),
  mAccessMode(AccessMode::BI),
  mBoundingMode(BoundingMode::FREE),
  mRepetitionFilter(false)
  {
    // edit value type, access mode, bounding mode and repetition filter attribute
    if (mData.valid())
    {
      TTSymbol objectName = mData.name();

      if (objectName == kTTSym_Mirror)
        objectName = TTMirrorPtr(mData.instance())->getName();

      if (objectName == "Data")
      {
        TTSymbol type;
        mData.get("type", type);

        if (type == kTTSym_none)
        {
          mValue = new Impulse();
          mValueType = AddressValue::Type::IMPULSE;
        }
        else if (type == kTTSym_generic)
        {
          mValue = new OSSIA::Tuple();
          mValueType = AddressValue::Type::TUPLE;
        }
        else if (type == kTTSym_boolean)
        {
          mValue = new OSSIA::Bool();
          mValueType = AddressValue::Type::BOOL;
        }
        else if (type == kTTSym_integer)
        {
          mValue = new OSSIA::Int();
          mValueType = AddressValue::Type::INT;
        }
        else if (type == kTTSym_decimal)
        {
          mValue = new OSSIA::Float();
          mValueType = AddressValue::Type::FLOAT;
        }
        else if (type == kTTSym_array)
        {
          mValue = new OSSIA::Tuple();
          mValueType = AddressValue::Type::TUPLE;
        }
        else if (type == kTTSym_string)
        {
          mValue = new OSSIA::String();
          mValueType = AddressValue::Type::STRING;
        }

        TTSymbol service;
        mData.get("service", service);

        if (service == kTTSym_parameter)
          mAccessMode = AccessMode::BI;
        else if (service == kTTSym_message)
          mAccessMode = AccessMode::SET;
        else if (service == kTTSym_return)
          mAccessMode = AccessMode::GET;
        
        TTValue range;
        mData.get("rangeBounds", range);
        
        if (type == kTTSym_none)
        {
          mDomain = new AddressDomain();
        }
        else if (type == kTTSym_generic)
        {
          mDomain = new AddressDomain();
        }
        else if (type == kTTSym_boolean)
        {
          if (range.size() == 2)
          {
            AddressValue * min = new OSSIA::Bool(range[0]);
            AddressValue * max = new OSSIA::Bool(range[1]);
            mDomain = new AddressDomain(min, max);
          }
        }
        else if (type == kTTSym_integer)
        {
          if (range.size() == 2)
          {
            AddressValue * min = new OSSIA::Int(range[0]);
            AddressValue * max = new OSSIA::Int(range[1]);
            mDomain = new AddressDomain(min, max);
          }
        }
        else if (type == kTTSym_decimal)
        {
          if (range.size() == 2)
          {
            AddressValue * min = new OSSIA::Float(range[0]);
            AddressValue * max = new OSSIA::Float(range[1]);
            mDomain = new AddressDomain(min, max);
          }
        }
        else if (type == kTTSym_array)
        {
          // we need to know the size of the array to setup the domain
          TTValue v;
          mData.get("value", v);
          
          std::vector<AddressValue*> tuple_min;
          std::vector<AddressValue*> tuple_max;
          for (int i = 0; i < v.size(); i++)
            tuple_min.push_back(new OSSIA::Float(range[0]));
            tuple_max.push_back(new OSSIA::Float(range[1]));
      
          mDomain = new AddressDomain(new OSSIA::Tuple(tuple_min), new OSSIA::Tuple(tuple_max));
        }
        else if (type == kTTSym_string)
        {
          // string values enumeration
          std::vector<AddressValue*> values;
          for (const auto & e : range)
          {
            TTSymbol s = e;
            values.push_back(new OSSIA::String(s.c_str()));
          }
          mDomain = new AddressDomain(new OSSIA::Impulse(), new OSSIA::Impulse(), values);
        }
        
        TTSymbol clipmode;
        mData.get("rangeClipmode", clipmode);

        if (clipmode == kTTSym_none)
          mBoundingMode = BoundingMode::FREE;
        else if (clipmode == kTTSym_low)
          mBoundingMode = BoundingMode::CLIP;
        else if (clipmode == kTTSym_high)
          mBoundingMode = BoundingMode::CLIP;
        else if (clipmode == kTTSym_both)
          mBoundingMode = BoundingMode::CLIP;
        else if (clipmode == kTTSym_wrap)
          mBoundingMode = BoundingMode::WRAP;
        else if (clipmode == kTTSym_fold)
          mBoundingMode = BoundingMode::FOLD;

        mData.get("repetitionFilter", mRepetitionFilter);

        // enable callback to be notified each time the value change
        TTObject    callback("callback");
        TTValue     args(TTPtr(this), mData);
        callback.set("baton", args);
        callback.set("function", TTPtr(&JamomaAddress::ValueCallback));

        TTAttributePtr attribute;
        mData.instance()->findAttribute("value", &attribute);
        attribute->registerObserverForNotifications(callback);
      }
    }
  }

  virtual ~JamomaAddress()
  {}

# pragma mark -
# pragma mark Value

  virtual bool updateValue() const override
  {
    TTValue v;
    mData.get("value", v);

    // clear former value
    delete mValue;

    // create new value
    mValue = convertTTValueIntoAddressValue(v, mValueType);

    return mValue != nullptr;
  }

  virtual AddressValue * getValue() const override
  {
    updateValue();
    return mValue;
  }

  virtual bool sendValue(const AddressValue * value) const override
  {
    TTValue v;
    convertAddressValueIntoTTValue(value, v);

    if (mData.name() == "Data")
      return !mData.send("Command", v);
    else
      return !mData.set("value", v);
  }

# pragma mark -
# pragma mark Network

  virtual const std::shared_ptr<Device> & getDevice() const override
  {
    return device;
  }

# pragma mark -
# pragma mark Accessors

  virtual AddressValue::Type getValueType() const override
  {
    return mValueType;
  }

  virtual AccessMode getAccessMode() const override
  {
    return mAccessMode;
  }

  virtual Address & setAccessMode(AccessMode) override
  {
    // note : it is not possible to change the service attribute of Data after its creation
    throw std::runtime_error("access mode cannot be set afterward");
    return *this;
  }
  
  virtual AddressDomain * getDomain() const override
  {
    return mDomain;
  }
  
  virtual Address & setDomain(AddressDomain * domain) override
  {
    mDomain = domain;
    return *this;
  }
  
  virtual BoundingMode getBoundingMode() const override
  {
    return mBoundingMode;
  }

  virtual Address & setBoundingMode(BoundingMode) override
  {
    if (mBoundingMode == BoundingMode::FREE)
      mData.set("rangeClipmode", kTTSym_none);
    else if (mBoundingMode == BoundingMode::CLIP)
      mData.set("rangeClipmode", kTTSym_both);
    else if (mBoundingMode == BoundingMode::WRAP)
      mData.set("rangeClipmode", kTTSym_wrap);
    else if (mBoundingMode == BoundingMode::FOLD)
      mData.set("rangeClipmode", kTTSym_fold);

    return *this;
  }

  virtual bool getRepetitionFilter() const override
  {
    return mRepetitionFilter;
  }

  virtual Address & setRepetitionFilter(bool repetitionFilter) override
  {
    mData.set("repetitionFilter", repetitionFilter);

    return *this;
  }

# pragma mark -
# pragma mark Callback

  virtual AddressValueCallback getAddressValueCallback() const override
  {
    return m_callback;
  }
 
  virtual void setAddressValueCallback(AddressValueCallback callback) override
  {
    m_callback = callback;
  }

# pragma mark -
# pragma mark Expression

  virtual bool evaluate() const override
  {
    return false;
  }

# pragma mark -
# pragma mark Implementation specific

private:

  static TTErr ValueCallback(const TTValue& baton, const TTValue& value)
  {
    JamomaAddress * self = static_cast<JamomaAddress*>(TTPtr(baton[0]));
    TTObject aData = baton[1];

    // check data object
    if (aData.instance() == self->mData.instance())
    {
      /*
      if (self->m_callback)
      {
        AddressValue * v = self->convertTTValueIntoAddressValue(value, self->mValueType);
        self->m_callback(v);
        delete v;
      */
        // DEBUG print the returned value
        TTLogMessage("address has been updated to %s\n", value.toString().data());
        
        return kTTErrNone;
      //}
    }

    return kTTErrGeneric;
  }

  AddressValue * convertTTValueIntoAddressValue(const TTValue& v, AddressValue::Type valueType) const
  {
    switch (valueType)
    {
      case AddressValue::Type::IMPULSE :
      {
        return new OSSIA::Impulse();
      }

      case AddressValue::Type::BOOL :
      {
        if (v.size() == 1)
          return new OSSIA::Bool(v[0]);

        return new OSSIA::Bool();
      }

      case AddressValue::Type::INT :
      {
        if (v.size() == 1)
          return new OSSIA::Int(v[0]);

        return new OSSIA::Int();
      }

      case AddressValue::Type::FLOAT :
      {
        if (v.size() == 1)
          return new OSSIA::Float(v[0]);

        return new OSSIA::Float();
      }

      case AddressValue::Type::CHAR :
      {
        if (v.size() == 1)
        {
          if (v[0].type() == kTypeString)
          {
            char* c_value = TTString(v[0]).data();
            return new OSSIA::Char(c_value[0]);
          }
        }

        return new OSSIA::Char();
      }

      case AddressValue::Type::STRING :
      {
        if (v.size() == 1)
        {
          if (v[0].type() == kTypeSymbol)
          {
            TTSymbol s_value = v[0];
            return new OSSIA::String(s_value.c_str());
          }
        }

        return new OSSIA::String();
      }

      case AddressValue::Type::TUPLE :
      {
        std::vector<AddressValue*> t_value;

        for (const auto & e : v)
        {
          AddressValue::Type type;

          if (e.type() == kTypeBoolean)
          {
            type = AddressValue::Type::BOOL;
          }
          else if (e.type() == kTypeInt8 || e.type() == kTypeUInt8 ||
                   e.type() == kTypeInt16 || e.type() == kTypeUInt16 ||
                   e.type() == kTypeInt32 || e.type() == kTypeUInt32 ||
                   e.type() == kTypeInt64 || e.type() == kTypeUInt64)
          {
            type = AddressValue::Type::INT;
          }
          else if (e.type() == kTypeFloat32 || e.type() == kTypeFloat64)
          {
            type = AddressValue::Type::FLOAT;
          }
          else if (e.type() == kTypeSymbol)
          {
            type = AddressValue::Type::STRING;
          }
          else
          {
            continue;
          }

          TTValue t(e);
          t_value.push_back(convertTTValueIntoAddressValue(t, type));
        }

        return new OSSIA::Tuple(t_value);
      }

      case AddressValue::Type::GENERIC :
      {
        return nullptr; // todo
      }
    }
  }

  void convertAddressValueIntoTTValue(const AddressValue * value, TTValue & v) const
  {
    if (value->getType() == AddressValue::Type::IMPULSE)
    {
      ;
    }
    else if (value->getType() == AddressValue::Type::BOOL)
    {
      Bool * b = (Bool*)value;
      v = TTBoolean(b->value);
    }
    else if (value->getType() == AddressValue::Type::INT)
    {
      Int * i = (Int*)value;
      v = TTInt32(i->value);
    }
    else if (value->getType() == AddressValue::Type::FLOAT)
    {
      Float * f = (Float*)value;
      v = TTFloat64(f->value);
    }
    else if (value->getType() == AddressValue::Type::CHAR)
    {
      Char * c = (Char*)value;
      v = TTSymbol(c->value);
    }
    else if (value->getType() == AddressValue::Type::STRING)
    {
      String * s = (String*)value;
      v = TTSymbol(s->value);
    }
    else if (value->getType() == AddressValue::Type::TUPLE)
    {
      Tuple * t = (Tuple*)value;

      for (const auto & e : t->value)
      {
        TTValue n;
        convertAddressValueIntoTTValue(e, n);

        if (n.size())
          v.append(n[0]);
      }
    }
    else if (value->getType() == AddressValue::Type::GENERIC)
    {
      ; // todo
    }
  }
};
