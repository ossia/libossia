#include "Editor/ExpressionComposition.h"
#include <algorithm>

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

  shared_ptr<ExpressionComposition> clone() const override
  {
    return make_shared<JamomaExpressionComposition>(this);
  }

  ~JamomaExpressionComposition()
  {}

# pragma mark -
# pragma mark Execution

  bool evaluate() const override
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
# pragma mark Callback Container

  void addCallback(ResultCallback* callback) override
  {
    callbacks().push_back(callback);

    if (callbacks().size() == 1)
      ;//! \todo start operands observation
  }

  void removeCallback(ResultCallback* callback) override
  {
    callbacks().erase(std::find(callbacks().begin(), callbacks().end(), callback));

    if (callbacks().size() == 0)
      ;//! \todo stop operands observation
  }

# pragma mark -
# pragma mark Accessors

  const shared_ptr<Expression> & getFirstOperand() const override
  {
    return mFirstExpression;
  }

  Operator getOperator() const override
  {
    return mOperator;
  }

  const shared_ptr<Expression> & getSecondOperand() const override
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
