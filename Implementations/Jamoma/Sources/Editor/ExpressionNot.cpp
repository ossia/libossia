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

  Expression::iterator    mResultCallbackIndex;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaExpressionNot(shared_ptr<Expression> expr) :
  mExpression(expr)
  {}

  JamomaExpressionNot(const JamomaExpressionNot * other)
  //! \todo mExpression(other->mExpression->clone())
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

  Expression::iterator addCallback(ResultCallback callback) override
  {
    auto it = CallbackContainer::addCallback(std::move(callback));

    if (callbacks().size() == 1)
    {
      // start expression observation
      mResultCallbackIndex = mExpression->addCallback(std::bind(&JamomaExpressionNot::resultCallback, this, _1));
    }

    return it;
  }

  void removeCallback(Expression::iterator callback) override
  {
    CallbackContainer::removeCallback(callback);

    if (callbacks().size() == 0)
    {
      // stop expression observation
      mExpression->removeCallback(mResultCallbackIndex);
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
      callback(!result);
  }
};

namespace OSSIA
{
  shared_ptr<ExpressionNot> ExpressionNot::create(shared_ptr<Expression> expr)
  {
    return make_shared<JamomaExpressionNot>(expr);
  }
}
