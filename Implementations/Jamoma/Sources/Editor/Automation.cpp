#include "TimeProcess.cpp"
#include "Editor/Automation.h"
#include "Network/Address.h"
#include "Network/AddressValue.h"

#include "TTScore.h"

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
  // Constructors, destructor, cloning
  JamomaAutomation()
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaAutomation(const JamomaAutomation * other)
  {}
  
  virtual ~JamomaAutomation()
  {}
  
  virtual shared_ptr<Automation> clone() const override
  {
    return nullptr;//shared_ptr<Automation>(new JamomaAutomation(this));
  }

  // Execution
  virtual void play(bool log = false, string name = "") const override
  {}

  // Accessors
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
