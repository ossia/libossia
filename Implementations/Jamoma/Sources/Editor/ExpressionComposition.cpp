#include "Editor/ExpressionComposition.h"
#include <algorithm>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaExpressionComposition final : public ExpressionComposition
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<Expression>  mFirstExpression;
  Operator                mOperator;
  shared_ptr<Expression>  mSecondExpression;

  Expression::iterator    mFirstResultCallbackIndex;
  Expression::iterator    mSecondResultCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionComposition(shared_ptr<Expression> expr1, Operator op, shared_ptr<Expression> expr2) :
  mFirstExpression(expr1),
  mOperator(op),
  mSecondExpression(expr2)
  {}

  JamomaExpressionComposition(const JamomaExpressionComposition * other) :
  //! \todo mFirstExpression(other->mFirstExpression->clone()),
  mOperator(other->mOperator)
  //! \todo mSecondExpression(other->mSecondExpression->clone())
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
    return do_evaluation(mFirstExpression->evaluate(), mSecondExpression->evaluate());
  }

# pragma mark -
# pragma mark Operator

  bool operator== (const Expression& expression) const override
  {
    if (expression.getType() == Expression::Type::COMPOSITION)
    {
      const JamomaExpressionComposition e = dynamic_cast<const JamomaExpressionComposition&>(expression);
      return *mFirstExpression == *e.mFirstExpression && mOperator == e.mOperator && *mSecondExpression == *e.mSecondExpression;
    }
    else
      return false;
  }

  bool operator!= (const Expression& expression) const override
  {
    if (expression.getType() == Expression::Type::COMPOSITION)
    {
      const JamomaExpressionComposition e = dynamic_cast<const JamomaExpressionComposition&>(expression);
      return *mFirstExpression != *e.mFirstExpression || mOperator != e.mOperator || *mSecondExpression != *e.mSecondExpression;
    }
    else
      return true;
  }

# pragma mark -
# pragma mark Callback Container

  Expression::iterator addCallback(ResultCallback callback) override
  {
    auto it = CallbackContainer::addCallback(std::move(callback));

    if (callbacks().size() == 1)
    {
      // start first expression observation
      mFirstResultCallbackIndex = mFirstExpression->addCallback(std::bind(&JamomaExpressionComposition::firstResultCallback, this, _1));

      // start second expression observation
      mSecondResultCallbackIndex = mSecondExpression->addCallback(std::bind(&JamomaExpressionComposition::secondResultCallback, this, _1));
    }

    return it;
  }

  void removeCallback(Expression::iterator callback) override
  {
    CallbackContainer::removeCallback(callback);

    if (callbacks().size() == 0)
    {
      // stop first expression observation
      mFirstExpression->removeCallback(mFirstResultCallbackIndex);

      // stop second expression observation
      mSecondExpression->removeCallback(mSecondResultCallbackIndex);
    }
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

private:

# pragma mark -
# pragma mark Implementation Specific

  bool do_evaluation(bool first, bool second) const
  {
    switch (mOperator)
    {
      case Operator::AND :
      {
        return first && second;
      }
      case Operator::OR :
      {
        return first || second;
      }
      case Operator::XOR :
      {
        return first ^ second;
      }
      default :
        return false;
    }
  }

  void firstResultCallback(bool first_result)
  {
    bool result = do_evaluation(first_result, mSecondExpression->evaluate());

    for (auto callback : callbacks())
      callback(result);
  }

  void secondResultCallback(bool second_result)
  {
    bool result = do_evaluation(mFirstExpression->evaluate(), second_result);

    for (auto callback : callbacks())
      callback(result);
  }

};

namespace OSSIA
{
  shared_ptr<ExpressionComposition> ExpressionComposition::create(shared_ptr<Expression> first_expr, Operator op, shared_ptr<Expression> second_expr)
  {
    return make_shared<JamomaExpressionComposition>(first_expr, op, second_expr);
  }
}
