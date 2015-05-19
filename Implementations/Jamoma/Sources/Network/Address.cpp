#include "Network/Address.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaAddress : public Address
{
  
private:
  
  // Implementation specific
  TTObject                    mData;
  AddressValue::Type          mValueType;
  AccessMode                  mAccessMode;
  BoundingMode                mBoundingMode;
  bool                        mRepetitionFilter;
  
  shared_ptr<Device> device;
  
public:
  
  // Constructor, destructor
  JamomaAddress(TTObject aData = TTObject()) :
  mData(aData),
  mValueType(AddressValue::Type::NONE),
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
          mValueType = AddressValue::Type::NONE;
        else if (type == kTTSym_generic)
          mValueType = AddressValue::Type::TUPLE;
        else if (type == kTTSym_boolean)
          mValueType = AddressValue::Type::BOOL;
        else if (type == kTTSym_integer)
          mValueType = AddressValue::Type::INT;
        else if (type == kTTSym_decimal)
          mValueType = AddressValue::Type::FLOAT;
        else if (type == kTTSym_array)
          mValueType = AddressValue::Type::TUPLE;
        else if (type == kTTSym_string)
          mValueType = AddressValue::Type::STRING;
        
        TTSymbol service;
        mData.get("service", service);
        
        if (service == kTTSym_parameter)
          mAccessMode = AccessMode::BI;
        else if (service == kTTSym_message)
          mAccessMode = AccessMode::SET;
        else if (service == kTTSym_return)
          mAccessMode = AccessMode::GET;
        
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
      }
    }
  }
  
  virtual ~JamomaAddress()
  {}
  
  // Network
  virtual const std::shared_ptr<Device> & getDevice() const override
  {
    return device;
  }
  
  virtual bool updateValue() const override
  {
    return false;
  }
  
  virtual bool sendValue(AddressValue value) const override
  {
    TTValue v;
    // todo : convert AddressValue into TTValue
    // todo : resolve the const problem
    // return !mData.send("Command", v);
    return false;
  }
  
  // Accessors
  virtual AddressValue getValue() const override
  {
    TTValue v;
    mData.get("value", v);
    // todo : convert TTValue into AddressValue
    return AddressValue();
  }
  
  virtual AddressValue::Type getValueType() const override
  {
    return mValueType;
  }
  
  virtual AccessMode getAccessMode() const override
  {
    return mAccessMode;
  }
  
  virtual BoundingMode getBoundingMode() const override
  {
    return mBoundingMode;
  }
  
  virtual bool getRepetitionFilter() const override
  {
    return mRepetitionFilter;
  }
  
  virtual Address & setAccessMode(AccessMode) override
  {
    return *this;
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
  
  virtual Address & setRepetitionFilter(bool repetitionFilter) override
  {
    mData.set("repetitionFilter", repetitionFilter);
    
    return *this;
  }
  
  // Expression
  virtual bool evaluate() const override
  {
    return false;
  }
  
};
