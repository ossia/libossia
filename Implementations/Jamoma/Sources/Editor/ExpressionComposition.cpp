#include "Editor/ExpressionComposition.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionComposition : public ExpressionComposition
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<Expression>  mFirstExpression;
  Operator                mOperator;
  shared_ptr<Expression>  mSecondExpression;

public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaExpressionComposition(shared_ptr<Expression> expr1, Operator op, shared_ptr<Expression> expr2) :
  mFirstExpression(expr1),
  mOperator(op),
  mSecondExpression(expr2)
  {}
  
  JamomaExpressionComposition(const JamomaExpressionComposition * other)
  {}
  
  virtual ~JamomaExpressionComposition()
  {}
  
  virtual shared_ptr<ExpressionComposition> clone() const override
  {
    return make_shared<JamomaExpressionComposition>(this);
  }

# pragma mark -
# pragma mark Execution
  
  virtual bool evaluate() const override
  {
    switch (mOperator)
    {
    case Operator::AND :
      {
        return mFirstExpression->evaluate() && mSecondExpression->evaluate();
      }
    case Operator::OR :
      {
        return mFirstExpression->evaluate() || mSecondExpression->evaluate();
      }
    case Operator::XOR :
      {
        return mFirstExpression->evaluate() ^ mSecondExpression->evaluate();
      }
    default :
        return false;
    }
  }

# pragma mark -
# pragma mark Accessors
  
  virtual const shared_ptr<Expression> & getFirstOperand() const override
  {
    return mFirstExpression;
  }
  
  virtual Operator getOperator() const override
  {
    return mOperator;
  }
  
  virtual const shared_ptr<Expression> & getSecondOperand() const
  {
    return mSecondExpression;
  }
};

namespace OSSIA
{
  shared_ptr<ExpressionComposition> ExpressionComposition::create(shared_ptr<Expression> first_expr, Operator op, shared_ptr<Expression> second_expr)
  {
    return make_shared<JamomaExpressionComposition>(first_expr, op, second_expr);
  }
}
