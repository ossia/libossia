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
  
  shared_ptr<ExpressionNot> clone() const override
  {
    return make_shared<JamomaExpressionNot>(this);
  }
  
  ~JamomaExpressionNot()
  {}

# pragma mark -
# pragma mark Execution
  
  bool evaluate() const override
  {
    return !mExpression->evaluate();
  }
  
# pragma mark -
# pragma mark Callback Container
  
  CallbackContainer<ResultCallback>::CallbackIterator addCallback(ResultCallback callback) override
  {
    callbacks().push_back(callback);
    
    if (callbacks().size() == 1)
      ;//! \todo start expression observation
    
    return callbacks().end();
  }
  
  void removeCallback(typename CallbackContainer<ResultCallback>::CallbackIterator iterator) override
  {
    callbacks().erase(iterator);
    
    if (callbacks().size() == 0)
      ;//! \todo stop expression observation
  }

# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<Expression> & getExpression() const override
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
