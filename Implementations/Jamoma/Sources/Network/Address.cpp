#include "Network/Address.h"

using namespace OSSIA;
using namespace std;

class JamomaAddress : public Address
{
  
private:
  
  // Implementation specific
  shared_ptr<Device> device;
  
public:

  // Constructor, destructor
  JamomaAddress(AddressValue::Type = AddressValue::Type::NONE)
  {}
  
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
  
  virtual bool sendValue(AddressValue) const override
  {
    return false;
  }

  // Accessors
  virtual AddressValue getValue() const override
  {
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
