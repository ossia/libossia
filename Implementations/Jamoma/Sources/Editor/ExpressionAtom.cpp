#include "Editor/ExpressionAtom.h"
#include "Network/AddressValue.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionAtom : public ExpressionAtom
{
  
private:
  // Implementation Specific
  shared_ptr<ExpressionValue> first_expr;
  shared_ptr<ExpressionValue> second_expr;
  
public:
  // Constructors, destructor, cloning
  JamomaExpressionAtom(std::shared_ptr<Address> addr, Operator op, std::shared_ptr<AddressValue> val)
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
  virtual const std::shared_ptr<ExpressionValue> & getFirstOperand() const override
  {
    return first_expr;
  }
  
  virtual const std::shared_ptr<ExpressionValue> & getSecondOperand() const override
  {
    return second_expr;
  }
  
  virtual Operator getOperator() const override
  {
    return Operator::IMPULSE;
  }

};

shared_ptr<ExpressionAtom> ExpressionAtom::create(std::shared_ptr<Address> addr, Operator op, std::shared_ptr<AddressValue> val)
{
  return shared_ptr<ExpressionAtom>(new JamomaExpressionAtom(addr, op, val));
}
