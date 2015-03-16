#include "Editor/ExpressionAtom.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionAtom : public ExpressionAtom
{
  
private:
  // Implementation Specific
  shared_ptr<Address> addr;

public:
  // Constructors, destructor, cloning
  JamomaExpressionAtom(std::shared_ptr<Address> addr, Operator op, AddressValue val)
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaExpressionAtom(const JamomaExpressionAtom * other)
  {}
  
  virtual ~JamomaExpressionAtom()
  {}
  
  virtual shared_ptr<ExpressionAtom> clone() const override
  {
    return shared_ptr<ExpressionAtom>(new JamomaExpressionAtom(this));
  }

  // Lecture
  virtual bool evaluate() const override
  {
    return true;
  }

  // Accessors
  virtual const shared_ptr<Address> & getAddress() const override
  {
    return addr;
  }
  
  virtual Operator getOperator() const override
  {
    return Operator::IMPULSE;
  }
  
  virtual AddressValue getOperand() const override
  {
    return AddressValue();
  }

};

shared_ptr<ExpressionAtom> ExpressionAtom::create(std::shared_ptr<Address> addr, Operator op, AddressValue val)
{
  return shared_ptr<ExpressionAtom>(new JamomaExpressionAtom(addr, op, val));
}
