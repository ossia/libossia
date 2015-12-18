#include "Editor/JamomaExpressionPulse.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<ExpressionPulse> ExpressionPulse::create(const Destination* destination)
  {
    return make_shared<JamomaExpressionPulse>(destination);
  }
}

JamomaExpressionPulse::JamomaExpressionPulse(const Destination* destination) :
mDestination(destination),
mResult(false)
{}

JamomaExpressionPulse::JamomaExpressionPulse(const JamomaExpressionPulse * other)
//! \todo mDestination(other->mDestination->clone())
{}

shared_ptr<ExpressionPulse> JamomaExpressionPulse::clone() const
{
  return make_shared<JamomaExpressionPulse>(this);
}

JamomaExpressionPulse::~JamomaExpressionPulse()
{}

ExpressionPulse::~ExpressionPulse()
{}

# pragma mark -
# pragma mark Execution

bool JamomaExpressionPulse::evaluate() const
{
  return mResult;
}

void JamomaExpressionPulse::update() const
{
  mResult = false;
}

# pragma mark -
# pragma mark Operator

bool JamomaExpressionPulse::operator== (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::PULSE)
  {
    const JamomaExpressionPulse e = dynamic_cast<const JamomaExpressionPulse&>(expression);
    return *mDestination == *e.mDestination;
  }
  else
    return false;
}

bool JamomaExpressionPulse::operator!= (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::PULSE)
  {
    const JamomaExpressionPulse e = dynamic_cast<const JamomaExpressionPulse&>(expression);
    return *mDestination != *e.mDestination;
  }
  else
    return true;
}

# pragma mark -
# pragma mark Callback Container

Expression::iterator JamomaExpressionPulse::addCallback(ResultCallback callback)
{
  auto it = CallbackContainer::addCallback(std::move(callback));
  
  if (callbacks().size() == 1)
  {
    // start destination observation
    if (mDestination->value->getAddress())
    {
      mResultCallbackIndex = mDestination->value->getAddress()->addCallback(std::bind(&JamomaExpressionPulse::destinationCallback, this, _1));
    }
  }
  
  return it;
}

void JamomaExpressionPulse::removeCallback(Expression::iterator callback)
{
  CallbackContainer::removeCallback(callback);
  
  if (callbacks().size() == 0)
  {
    // stop destination observation
    if (mDestination->value->getAddress())
    {
      mDestination->value->getAddress()->removeCallback(mResultCallbackIndex);
    }
  }
}

# pragma mark -
# pragma mark Accessors

const Destination* JamomaExpressionPulse::getDestination() const
{
  return mDestination;
}

# pragma mark -
# pragma mark Implementation Specific

void JamomaExpressionPulse::destinationCallback(const Value * value)
{
  mResult = true;
  
  for (auto callback : callbacks())
    callback(true);
}
