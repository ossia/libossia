#include "ExpressionNot_impl.hpp"

namespace impl
{

JamomaExpressionNot::JamomaExpressionNot(std::shared_ptr<Expression> expr) :
mExpression(expr)
{
}

JamomaExpressionNot::~JamomaExpressionNot()
{
  // TODO REMOVE ALL CALLBACKS
}

# pragma mark -
# pragma mark Execution

bool JamomaExpressionNot::evaluate() const
{
  return !mExpression->evaluate();
}

void JamomaExpressionNot::update() const
{
  mExpression->update();
}

# pragma mark -
# pragma mark Operator

bool JamomaExpressionNot::operator== (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::NOT)
  {
    const JamomaExpressionNot e = dynamic_cast<const JamomaExpressionNot&>(expression);
    return *mExpression == *e.mExpression;
  }
  else
    return false;
}

bool JamomaExpressionNot::operator!= (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::NOT)
  {
    const JamomaExpressionNot e = dynamic_cast<const JamomaExpressionNot&>(expression);
    return *mExpression != *e.mExpression;
  }
  else
    return true;
}

# pragma mark -
# pragma mark Accessors

const std::shared_ptr<Expression> & JamomaExpressionNot::getExpression() const
{
  return mExpression;
}

# pragma mark -
# pragma mark Implementation Specific

void JamomaExpressionNot::onFirstCallbackAdded()
{
  mResultCallbackIndex = mExpression->addCallback([&] (bool result) { resultCallback(result); });
}

void JamomaExpressionNot::onRemovingLastCallback()
{
  mExpression->removeCallback(mResultCallbackIndex);
}

void JamomaExpressionNot::resultCallback(bool result)
{
  send(!result);
}

}
