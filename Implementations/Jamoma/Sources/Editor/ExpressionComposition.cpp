#include "Editor/ExpressionComposition.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionComposition : public ExpressionComposition
{
  
private:
  
  // Implementation specific
  shared_ptr<Expression> first_expr;
  shared_ptr<Expression> second_expr;

public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaExpressionComposition(shared_ptr<Expression> first_expr, Operator op, shared_ptr<Expression> second_expr)
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
    return true;
  }

# pragma mark -
# pragma mark Accessors
  
  virtual const shared_ptr<Expression> & getFirstOperand() const override
  {
    return first_expr;
  }
  
  virtual Operator getOperator() const override
  {
    return Operator::AND;
  }
  
  virtual const shared_ptr<Expression> & getSecondOperand() const
  {
    return second_expr;
  }
};

shared_ptr<ExpressionComposition> ExpressionComposition::create(std::shared_ptr<Expression> first_expr, Operator op, std::shared_ptr<Expression> second_expr)
{
  return make_shared<JamomaExpressionComposition>(first_expr, op, second_expr);
}
