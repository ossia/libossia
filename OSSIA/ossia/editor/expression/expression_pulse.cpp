#include <ossia/editor/expression/expression_pulse.hpp>
#include <ossia/editor/value/destination.hpp>
namespace ossia
{
namespace expressions
{

expression_pulse::expression_pulse(const Destination& destination)
    : mDestination(destination), mResult(false)
{
  // start destination observation
  if (const auto& addr = mDestination.value->getAddress())
  {
    mDestinationCallbackIndex = addr->addCallback(
        [&](const ossia::value& result) { destinationCallback(result); });
  }
}

expression_pulse::~expression_pulse()
{
  // stop destination observation
  if (const auto& addr = mDestination.value->getAddress())
  {
    addr->removeCallback(mDestinationCallbackIndex);
  }
}

bool expression_pulse::evaluate() const
{
  return mResult;
}

void expression_pulse::update() const
{
  // the result will be false until the next
  // #JamomaExpressionPulse::destinationCallback call
  mResult = false;
}

void expression_pulse::onFirstCallbackAdded()
{
}

void expression_pulse::onRemovingLastCallback()
{
}

const Destination& expression_pulse::getDestination() const
{
  return mDestination;
}

void expression_pulse::destinationCallback(const ossia::value& value)
{
  mResult = true;
  send(true);
}
}
}
