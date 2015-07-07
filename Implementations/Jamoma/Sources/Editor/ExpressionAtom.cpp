#include "Editor/ExpressionAtom.h"
#include "Editor/Value.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionAtom : public ExpressionAtom
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  Value*    mFirstExpression;
  Operator  mOperator;
  Value*    mSecondExpression;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaExpressionAtom(const Value* expr1, Operator op, const Value* expr2) :
  mFirstExpression(expr1->clone()),
  mOperator(op),
  mSecondExpression(expr2->clone())
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
        return mFirstExpression == mSecondExpression;
      }
    case Operator::GREATER_THAN :
      {
        return mFirstExpression > mSecondExpression;
      }
    case Operator::LOWER_THAN :
      {
        return mFirstExpression < mSecondExpression;
      }
    case Operator::GREATER_THAN_OR_EQUAL :
      {
        return mFirstExpression >= mSecondExpression;
      }
    case Operator::LOWER_THAN_OR_EQUAL :
      {
        return mFirstExpression <= mSecondExpression;
      }
    default :
        return false;
    }
  }

# pragma mark -
# pragma mark Accessors
  
  virtual const Value* getFirstOperand() const override
  {
    return mFirstExpression;
  }
  
  virtual Operator getOperator() const override
  {
    return mOperator;
  }
  
  virtual const Value* getSecondOperand() const override
  {
    return mSecondExpression;
  }

};

namespace OSSIA
{
  shared_ptr<ExpressionAtom> ExpressionAtom::create(const Value* expr1, Operator op, const Value* expr2)
  {
    return make_shared<JamomaExpressionAtom>(expr1, op, expr2);
  }
}