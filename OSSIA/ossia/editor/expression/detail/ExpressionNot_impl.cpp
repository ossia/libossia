#include "ExpressionNot_impl.hpp"

namespace impl
{

JamomaExpressionNot::JamomaExpressionNot(std::shared_ptr<Expression> expr) :
mExpression(expr)
{}

JamomaExpressionNot::JamomaExpressionNot(const JamomaExpressionNot * other)
//! \todo mExpression(other->mExpression->clone())
{}

std::shared_ptr<ExpressionNot> JamomaExpressionNot::clone() const
{
  return std::make_shared<JamomaExpressionNot>(this);
}

JamomaExpressionNot::~JamomaExpressionNot()
{}

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
# pragma mark Callback Container

Expression::iterator JamomaExpressionNot::addCallback(ResultCallback callback)
{
  auto it = CallbackContainer::addCallback(std::move(callback));

  if (callbacks().size() == 1)
  {
    // start expression observation
    mResultCallbackIndex = mExpression->addCallback([&] (bool result) { resultCallback(result); });
  }

  return it;
}

void JamomaExpressionNot::removeCallback(Expression::iterator callback)
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

const std::shared_ptr<Expression> & JamomaExpressionNot::getExpression() const
{
  return mExpression;
}

# pragma mark -
# pragma mark Implementation Specific

void JamomaExpressionNot::resultCallback(bool result)
{
  send(!result);
}

}
