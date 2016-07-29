#include "ExpressionPulse_impl.hpp"
#include <ossia/editor/value/value.hpp>

namespace impl
{

JamomaExpressionPulse::JamomaExpressionPulse(const Destination& destination) :
mDestination(destination),
mResult(false)
{
}

JamomaExpressionPulse::~JamomaExpressionPulse()
{
}


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
    auto& e = dynamic_cast<const JamomaExpressionPulse&>(expression);
    return Value{mDestination} == Value{e.mDestination};
  }
  else
    return false;
}

bool JamomaExpressionPulse::operator!= (const Expression& expression) const
{
  if (expression.getType() == Expression::Type::PULSE)
  {
    auto& e = dynamic_cast<const JamomaExpressionPulse&>(expression);
    return Value{mDestination} != Value{e.mDestination};
  }
  else
    return true;
}

# pragma mark -
# pragma mark Callback Container
void JamomaExpressionPulse::onFirstCallbackAdded()
{
  // start destination observation
  if (const auto& addr = mDestination.value->getAddress())
  {
    mDestinationCallbackIndex = addr->addCallback(
                                  [&] (const OSSIA::Value& result) { destinationCallback(result); });
  }
}

void JamomaExpressionPulse::onRemovingLastCallback()
{
  // stop destination observation
  if (const auto& addr = mDestination.value->getAddress())
  {
    addr->removeCallback(mDestinationCallbackIndex);
  }
}

# pragma mark -
# pragma mark Accessors

const Destination& JamomaExpressionPulse::getDestination() const
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

}
