#include "Editor/ExpressionNot.h"

using namespace OSSIA;
using namespace std;

class JamomaExpressionNot : public ExpressionNot
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<Expression>  mExpression;

public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaExpressionNot(shared_ptr<Expression> expr) :
  mExpression(expr)
  {}
  
  JamomaExpressionNot(const JamomaExpressionNot * other) :
  mExpression(other->mExpression)
  {}
  
  virtual shared_ptr<ExpressionNot> clone() const override
  {
    return make_shared<JamomaExpressionNot>(this);
  }
  
  virtual ~JamomaExpressionNot()
  {}

# pragma mark -
# pragma mark Execution
  
  virtual bool evaluate() const override
  {
    return !mExpression->evaluate();
  }

# pragma mark -
# pragma mark Accessors
  
  virtual const shared_ptr<Expression> & getExpression() const override
  {
    return mExpression;
  }
};

namespace OSSIA
{
  shared_ptr<ExpressionNot> ExpressionNot::create(shared_ptr<Expression> expr)
  {
    return make_shared<JamomaExpressionNot>(expr);
  }
}
