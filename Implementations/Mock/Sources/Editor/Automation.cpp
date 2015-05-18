#include "TimeProcess.cpp"
#include "../Misc/Container.cpp"
#include "Editor/Automation.h"
#include "Network/AddressValue.h"

using namespace OSSIA;
using namespace std;

class MockAutomation :
    public Automation, public MockTimeProcess, public MockContainer<Address> {

public:

  // Dummy members
  shared_ptr<Curve> curve;
  shared_ptr<Address> addr;

  // Constructors, destructor, cloning
  MockAutomation() {}
  MockAutomation(const MockAutomation * other) :
      curve{other->curve},
      addr{other->addr}  {}

  virtual ~MockAutomation() {}
  virtual shared_ptr<Automation> clone() const override {
    return shared_ptr<Automation>(new MockAutomation(this));
  }

  // Lecture
  virtual void play(bool log = false, string name = "") const override {}

  // Accessors

  virtual AddressValue getStartValue() const override {return AddressValue();}
  virtual void setStartValue(AddressValue) override {}
  virtual AddressValue getEndValue() const override {return AddressValue();}
  virtual void setEndValue(AddressValue) override {}
  virtual const shared_ptr<Curve> & getCurve() const override {return curve;}
  virtual void setCurve(shared_ptr<Curve> cv) override {curve = cv;}
  virtual const shared_ptr<Address> & getInputAdress() const override {return addr;}
  virtual void setInputAddress(shared_ptr<Address> a) override {addr = a;}
    //TODO : why 'input'address ?
};

shared_ptr<Automation> Automation::create() {
  return shared_ptr<Automation>(new MockAutomation());
}
