#include "Network/Address.h"

#include "TTModular.h"

using namespace OSSIA;
using namespace std;

class JamomaAddress : public Address
{
  
private:
  
  // Implementation specific
  TTObject  mObject;
  shared_ptr<Device> device;
  
public:

  // Constructor, destructor
  JamomaAddress(AddressValue::Type = AddressValue::Type::NONE)
  {
    ; // nothing to do as we need to wait the owner sets mObject member
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
    // return !mObject.send("Command", v);
    return false;
  }

  // Accessors
  virtual AddressValue getValue() const override
  {
    TTValue v;
    mObject.get("value", v);
    // todo : convert TTValue into AddressValue
    return AddressValue();
  }
  
  virtual AddressValue::Type getValueType() const override
  {
    return AddressValue::Type::NONE;
  }

  virtual AccessMode getAccessMode() const override
  {
    return AccessMode::BI;
  }
  
  virtual BoundingMode getBoundingMode() const override
  {
    return BoundingMode::FREE;
  }
  
  virtual bool getRepetitionFilter() const override
  {
    return false;
  }
  
  virtual Address & setAccessMode(AccessMode) override
  {
    return *this;
  }
  
  virtual Address & setBoundingMode(BoundingMode) override
  {
    return *this;
  }
  
  virtual Address & setRepetitionFilter(bool) override
  {
    return *this;
  }

  // Expression
  virtual bool evaluate() const override
  {
    return false;
  }

};
