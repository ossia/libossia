#include "Editor/ExpressionComposition.h"

#include "TTScore.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionComposition : public ExpressionComposition
{
  
private:
  
  // Implementation specific
  shared_ptr<Expression> first_expr;
  shared_ptr<Expression> second_expr;

public:
  // Constructors, destructor, cloning
  JamomaExpressionComposition(shared_ptr<Expression> first_expr, Operator op, shared_ptr<Expression> second_expr)
  {
    // todo : we shouldn't init each time we create an object ...
    TTFoundationInit("/usr/local/jamoma/");
    TTModularInit("/usr/local/jamoma/");
    TTScoreInit("/usr/local/jamoma/");
  }
  
  JamomaExpressionComposition(const JamomaExpressionComposition * other)
  {}
  
  virtual ~JamomaExpressionComposition()
  {}
  
  virtual shared_ptr<ExpressionComposition> clone() const override
  {
    return shared_ptr<ExpressionComposition>(new JamomaExpressionComposition(this));
  }

  // Execution
  virtual bool evaluate() const override
  {
    return true;
  }

  // Accessors
  virtual const shared_ptr<Expression> & getFirstOperand() const override
  {
    return first_expr;
  }
  
  virtual const shared_ptr<Expression> & getSecondOperand() const
  {
    return second_expr;
  }
  
  virtual Operator getOperator() const override
  {
    return Operator::AND;
  }

};

shared_ptr<ExpressionComposition> ExpressionComposition::create(std::shared_ptr<Expression> first_expr, Operator op, std::shared_ptr<Expression> second_expr)
{
  return shared_ptr<ExpressionComposition>(new JamomaExpressionComposition(first_expr, op, second_expr));
}
