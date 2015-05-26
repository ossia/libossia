#include "Network/Address.h"

using namespace OSSIA;
using namespace std;

class MockAddress : public Address {

public:

  // Dummy members
  shared_ptr<Device> device;

  // Constructor, destructor
  MockAddress(AddressValue::Type = AddressValue::Type::NONE) {}
  virtual ~MockAddress() {}

  // Network
  virtual const std::shared_ptr<Device> & getDevice() const override {
    return device;
  }
  virtual bool updateValue() const override {return false;}
  virtual bool sendValue(AddressValue*) const override {return false;}

  virtual AddressValue * getValue() const override {return new AddressValue;}
  virtual AddressValue::Type getValueType() const override {
    return AddressValue::Type::NONE;
  }

  virtual AccessMode getAccessMode() const override {return AccessMode::BI;}
  virtual BoundingMode getBoundingMode() const override {
    return BoundingMode::FREE;
  }
  virtual bool getRepetitionFilter() const override {return false;}
  virtual Address & setAccessMode(AccessMode) override {return *this;}
  virtual Address & setBoundingMode(BoundingMode) override {return *this;}
  virtual Address & setRepetitionFilter(bool) override {return *this;}

  // Expression
  virtual bool evaluate() const override {return false;}

};
