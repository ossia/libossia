#include "Editor/ExpressionNot.h"
#include <algorithm>
using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaExpressionNot : public ExpressionNot
{

private:

# pragma mark -
# pragma mark Implementation specific

  shared_ptr<Expression>  mExpression;
  
  ResultCallback          mResultCallback;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionNot(shared_ptr<Expression> expr) :
  mExpression(expr),
  mResultCallback(std::bind(&JamomaExpressionNot::resultCallback, this, _1))
  {}

  JamomaExpressionNot(const JamomaExpressionNot * other) :
  //! \todo mExpression(other->mExpression->clone()),
  mResultCallback(std::bind(&JamomaExpressionNot::resultCallback, this, _1))
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

  void addCallback(ResultCallback* callback) override
  {
    callbacks().push_back(callback);

    if (callbacks().size() == 1)
    {
      // start expression observation
      mExpression->addCallback(&mResultCallback);
    }
  }

  void removeCallback(ResultCallback* callback) override
  {
    callbacks().erase(std::find(callbacks().begin(), callbacks().end(), callback));

    if (callbacks().size() == 0)
    {
      // stop expression observation
      mExpression->removeCallback(&mResultCallback);
    }
  }

# pragma mark -
# pragma mark Accessors

  const shared_ptr<Expression> & getExpression() const override
  {
    return mExpression;
  }
  
private:
  
# pragma mark -
# pragma mark Implementation Specific
  
  void resultCallback(bool result)
  {
    for (auto callback : callbacks())
      (*callback)(!result);
  }
};

namespace OSSIA
{
  shared_ptr<ExpressionNot> ExpressionNot::create(shared_ptr<Expression> expr)
  {
    return make_shared<JamomaExpressionNot>(expr);
  }
}
