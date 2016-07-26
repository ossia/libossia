#include "ExpressionPulse_impl.hpp"
#include <Editor/Value/Value.h>

namespace impl
{

JamomaExpressionPulse::JamomaExpressionPulse(const Destination& destination) :
mDestination(destination),
mResult(false)
{
  // start destination observation
  if (const auto& addr = mDestination.value->getAddress())
  {
    mDestinationCallbackIndex = addr->addCallback(
          [&] (const OSSIA::Value& result) { destinationCallback(result); });
  }
}

JamomaExpressionPulse::JamomaExpressionPulse(const JamomaExpressionPulse& other)
//! \todo mDestination(other->mDestination.clone())
{}

shared_ptr<ExpressionPulse> JamomaExpressionPulse::clone() const
{
  return make_shared<JamomaExpressionPulse>(*this);
}

JamomaExpressionPulse::~JamomaExpressionPulse()
{
  // stop destination observation
  if (const auto& addr = mDestination.value->getAddress())
  {
    addr->removeCallback(mDestinationCallbackIndex);
  }
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
