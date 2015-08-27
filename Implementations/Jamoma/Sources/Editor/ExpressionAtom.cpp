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
  
  shared_ptr<ExpressionAtom> clone() const override
  {
    return make_shared<JamomaExpressionAtom>(this);
  }
  
  ~JamomaExpressionAtom()
  {}

# pragma mark -
# pragma mark Execution
  
  bool evaluate() const override
  {
    switch (mOperator)
    {
      case Operator::EQUAL :
      {
        return (*mFirstValue) == (*mSecondValue);
      }
      case Operator::DIFFERENT :
      {
        return (*mFirstValue) != (*mSecondValue);
      }
      case Operator::GREATER_THAN :
      {
        return (*mFirstValue) > (*mSecondValue);
      }
      case Operator::LOWER_THAN :
      {
        return (*mFirstValue) < (*mSecondValue);
      }
      case Operator::GREATER_THAN_OR_EQUAL :
      {
        return (*mFirstValue) >= (*mSecondValue);
      }
      case Operator::LOWER_THAN_OR_EQUAL :
      {
        return (*mFirstValue) <= (*mSecondValue);
      }
      default :
        return false;
    }
  }
  
# pragma mark -
# pragma mark Callback Container
  
  CallbackContainer<ResultCallback>::CallbackIterator addCallback(ResultCallback callback) override
  {
    callbacks().push_back(callback);
    
    if (callbacks().size() == 1)
      ;//! \todo start operands observation if they are Destinations
    
    return callbacks().end();
  }
  
  void removeCallback(typename CallbackContainer<ResultCallback>::CallbackIterator iterator) override
  {
    callbacks().erase(iterator);
    
    if (callbacks().size() == 0)
      ;//! \todo stop operands observation if they are Destinations
  }

# pragma mark -
# pragma mark Accessors
  
  const Value* getFirstOperand() const override
  {
    return mFirstValue;
  }
  
  Operator getOperator() const override
  {
    return mOperator;
  }
  
  const Value* getSecondOperand() const override
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