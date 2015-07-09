#include "Editor/ExpressionAtom.h"
#include "Editor/Value.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionAtom : public ExpressionAtom
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  Value*    mFirstValue;
  Operator  mOperator;
  Value*    mSecondValue;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaExpressionAtom(const Value* value1, Operator op, const Value* value2) :
  mFirstValue(value1->clone()),
  mOperator(op),
  mSecondValue(value2->clone())
  {}
  
  JamomaExpressionAtom(const JamomaExpressionAtom * other)
  {}
  
  virtual ~JamomaExpressionAtom()
  {}
  
  virtual shared_ptr<ExpressionAtom> clone() const override
  {
    return make_shared<JamomaExpressionAtom>(this);
  }

# pragma mark -
# pragma mark Execution
  
  virtual bool evaluate() const override
  {
    switch (mOperator)
    {
      case Operator::EQUAL :
      {
        return mFirstValue == mSecondValue;
      }
    case Operator::GREATER_THAN :
      {
        return mFirstValue > mSecondValue;
      }
    case Operator::LOWER_THAN :
      {
        return mFirstValue < mSecondValue;
      }
    case Operator::GREATER_THAN_OR_EQUAL :
      {
        return mFirstValue >= mSecondValue;
      }
    case Operator::LOWER_THAN_OR_EQUAL :
      {
        return mFirstValue <= mSecondValue;
      }
    default :
        return false;
    }
  }

# pragma mark -
# pragma mark Accessors
  
  virtual const Value* getFirstOperand() const override
  {
    return mFirstValue;
  }
  
  virtual Operator getOperator() const override
  {
    return mOperator;
  }
  
  virtual const Value* getSecondOperand() const override
  {
    return mSecondValue;
  }

};

namespace OSSIA
{
  shared_ptr<ExpressionAtom> ExpressionAtom::create(const Value* value1, Operator op, const Value* value2)
  {
    return make_shared<JamomaExpressionAtom>(value1, op, value2);
  }
}