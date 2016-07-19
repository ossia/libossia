#include "Editor/JamomaExpressionPulse.h"
#include <Editor/Value/Value.h>

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
mDestination((Destination*)destination->clone()),
mResult(false)
{
  // start destination observation
  if (mDestination->value->getAddress())
  {
    mDestinationCallbackIndex = mDestination->value->getAddress()->addCallback(
          [&] (const OSSIA::Value& result) { destinationCallback(result); });
  }
}

JamomaExpressionPulse::JamomaExpressionPulse(const JamomaExpressionPulse * other)
//! \todo mDestination(other->mDestination->clone())
{}

shared_ptr<ExpressionPulse> JamomaExpressionPulse::clone() const
{
  return make_shared<JamomaExpressionPulse>(this);
}

JamomaExpressionPulse::~JamomaExpressionPulse()
{
  // stop destination observation
  if (mDestination->value->getAddress())
  {
    mDestination->value->getAddress()->removeCallback(mDestinationCallbackIndex);
  }
}

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
  // the result will be false until the next #JamomaExpressionPulse::destinationCallback call
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
  return it;
}

void JamomaExpressionPulse::removeCallback(Expression::iterator callback)
{
  CallbackContainer::removeCallback(callback);
}

# pragma mark -
# pragma mark Accessors

const Destination* JamomaExpressionPulse::getDestination() const
{
  return mDestination;
}

# pragma mark -
# pragma mark Implementation Specific

void JamomaExpressionPulse::destinationCallback(const Value& value)
{
  mResult = true;
  send(true);
}
