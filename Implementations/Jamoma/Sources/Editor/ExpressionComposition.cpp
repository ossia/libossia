#include "Editor/ExpressionComposition.h"
#include <algorithm>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaExpressionComposition : public ExpressionComposition
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<Expression>  mFirstExpression;
  Operator                mOperator;
  shared_ptr<Expression>  mSecondExpression;
  
  ResultCallback          mFirstResultCallback;
  ResultCallback          mSecondResultCallback;

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
    return do_evaluation(mFirstExpression->evaluate(), mSecondExpression->evaluate());
  }

# pragma mark -
# pragma mark Callback Container

  void addCallback(ResultCallback* callback) override
  {
    callbacks().push_back(callback);

    if (callbacks().size() == 1)
    {
      // start first expression observation
      mFirstResultCallback = std::bind(&JamomaExpressionComposition::firstResultCallback, this, _1);
      mFirstExpression->addCallback(&mFirstResultCallback);
      
      // start second expression observation
      mSecondResultCallback = std::bind(&JamomaExpressionComposition::secondResultCallback, this, _1);
      mSecondExpression->addCallback(&mSecondResultCallback);
    }
  }

  void removeCallback(ResultCallback* callback) override
  {
    callbacks().erase(std::find(callbacks().begin(), callbacks().end(), callback));

    if (callbacks().size() == 0)
    {
      // stop first expression observation
      mFirstExpression->removeCallback(&mFirstResultCallback);
      
      // stop second expression observation
      mSecondExpression->removeCallback(&mSecondResultCallback);
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
      (*callback)(result);
  }
  
  void secondResultCallback(bool second_result)
  {
    bool result = do_evaluation(mFirstExpression->evaluate(), second_result);
    
    for (auto callback : callbacks())
      (*callback)(result);
  }

};

namespace OSSIA
{
  shared_ptr<ExpressionComposition> ExpressionComposition::create(shared_ptr<Expression> first_expr, Operator op, shared_ptr<Expression> second_expr)
  {
    return make_shared<JamomaExpressionComposition>(first_expr, op, second_expr);
  }
}
