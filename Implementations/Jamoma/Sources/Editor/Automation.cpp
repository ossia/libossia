#include "TimeProcess.cpp"
#include "Editor/Automation.h"
#include "Network/Address.h"
#include "Network/AddressValue.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;


class JamomaAutomation : virtual Automation, virtual JamomaTimeProcess
{
  
private:
  
  // Implementation Specific
  shared_ptr<Curve> curve;
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

  // Lecture
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
  
  virtual const shared_ptr<Curve> & getCurve() const override
  {
    return curve;
  }
  
  virtual void setCurve(shared_ptr<Curve>) override
  {}
  
  virtual const shared_ptr<Address> & getInputAdress() const override
  {
    return addr;
  }
  
  virtual void setInputAddress(shared_ptr<Address>) override
  {}

  // Std container
  virtual iterator begin() override
  {
    return iterator();
  }
  
  virtual iterator end() override
  {
    return iterator();
  }
  
  virtual const_iterator cbegin() const override
  {
    return iterator();
  }
  
  virtual const_iterator cend() const override
  {
    return iterator();
  }
  
  virtual size_type size() const override
  {
    return size_type();
  }
  
  virtual bool empty() const override
  {
    return true;
  }
  
  virtual shared_ptr<Address> & front() override
  {
    return element;
  }
  virtual const shared_ptr<Address> & front() const override
  {
    return element;
  }
  
  virtual shared_ptr<Address> & back() override
  {
    return element;
  }
  
  virtual const shared_ptr<Address> & back() const override
  {
    return element;
  }
  
  virtual iterator insert(const_iterator where,
			  shared_ptr<Address> what) override
  {
    return iterator();
  }
  
  virtual iterator erase(const_iterator which) override
  {
    return iterator();
  }
  
  virtual iterator erase(const_iterator first,
			 const_iterator last) override
  {
    return iterator();
  }

};

shared_ptr<Automation> Automation::create()
{
  return nullptr;//shared_ptr<Automation>(new JamomaAutomation());
}
