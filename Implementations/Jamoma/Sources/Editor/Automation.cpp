#include "TimeProcess.cpp"
#include "Editor/Automation.h"
#include "Network/Address.h"
#include "Network/AddressValue.h"

using namespace OSSIA;
using namespace std;


class JamomaAutomation : virtual Automation<double>, virtual JamomaTimeProcess
{
  
private:
  
  // Implementation Specific
  shared_ptr<Curve<double>> curve;
  shared_ptr<Address> addr;
  shared_ptr<Address> element;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  JamomaAutomation()
  {}
  
  JamomaAutomation(const JamomaAutomation * other)
  {}
  
  virtual ~JamomaAutomation()
  {}
  
  virtual shared_ptr<Automation> clone() const override
  {
    return nullptr;//shared_ptr<Automation>(new JamomaAutomation(this));
  }

# pragma mark -
# pragma mark Execution
  virtual void play(bool log = false, string name = "") const override
  {}

# pragma mark -
# pragma mark Accessors
  virtual AddressValue getStartValue() const override
  {
    return AddressValue();
  }
  
  virtual void setStartValue(AddressValue) override
  {}
  
  virtual AddressValue getEndValue() const override
  {
    return AddressValue();
  }
  
  virtual void setEndValue(AddressValue) override
  {}
  
  virtual const shared_ptr<Curve<double>> & getCurve() const override
  {
    return curve;
  }
  
  virtual void setCurve(shared_ptr<Curve<double>>) override
  {}
/*
  virtual const shared_ptr<Address> & getInputAdress() const override
  {
    return addr;
  }
*/  
  virtual void setInputAddress(shared_ptr<Address>) override
  {}
};

template<> shared_ptr<Automation<double>> Automation<double>::create()
{
  return nullptr;//shared_ptr<Automation>(new JamomaAutomation());
}
