#include "TimeEvent_impl.hpp"
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/state/state_element.hpp>
namespace impl
{

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(time_event::ExecutionCallback callback,
                                 std::shared_ptr<time_node> aTimeNode,
                                 std::shared_ptr<expression_base> anExpression) :
mCallback(callback),
mTimeNode(aTimeNode),
mStatus(time_event::Status::NONE),
mExpression(anExpression)
{
}

JamomaTimeEvent::~JamomaTimeEvent()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeEvent::setCallback(time_event::ExecutionCallback callback)
{
  mCallback = callback;
}

void JamomaTimeEvent::happen()
{
  if (mStatus != time_event::Status::PENDING)
    throw std::runtime_error("only PENDING event can happens");

  mStatus = time_event::Status::HAPPENED;

  // stop previous TimeConstraints
  for (auto& timeConstraint : previousTimeConstraints())
  {
    timeConstraint->stop();
  }

  // setup next TimeConstraints
  for (auto& timeConstraint : nextTimeConstraints())
  {
    timeConstraint->start();
  }

  if (mCallback)
      (mCallback)(mStatus);
}

void JamomaTimeEvent::dispose()
{
  if (mStatus == time_event::Status::HAPPENED)
    throw std::runtime_error("HAPPENED event cannot be disposed");

  mStatus = time_event::Status::DISPOSED;

  // dispose next TimeConstraints end event if everything is disposed before
  for (auto& nextTimeConstraint : nextTimeConstraints())
  {
    bool dispose = true;

    for (auto& previousTimeConstraint : nextTimeConstraint->getEndEvent()->previousTimeConstraints())
    {
      if (previousTimeConstraint->getStartEvent()->getStatus() != time_event::Status::DISPOSED)
      {
        dispose = false;
        break;
      }
    }

    if (dispose)
      nextTimeConstraint->getEndEvent()->dispose();
  }

  if (mCallback)
      (mCallback)(mStatus);
}

# pragma mark -
# pragma mark Edition

void JamomaTimeEvent::addState(State&& state)
{
    mState.add(StateElement{std::move(state)});
}

void JamomaTimeEvent::removeState(const State& state)
{
    mState.remove(state);
}

# pragma mark -
# pragma mark Accessors

const std::shared_ptr<time_node> & JamomaTimeEvent::getTimeNode() const
{
  return mTimeNode;
}

const State& JamomaTimeEvent::getState() const
{
  return mState;
}

const std::shared_ptr<expression_base> & JamomaTimeEvent::getExpression() const
{
  return mExpression;
}

time_event & JamomaTimeEvent::setExpression(std::shared_ptr<expression_base> exp)
{
  assert(exp != nullptr);

  mExpression = std::move(exp);

  return *this;
}

time_event::Status JamomaTimeEvent::getStatus() const
{
  return mStatus;
}

# pragma mark -
# pragma mark Implementation specific

void JamomaTimeEvent::setStatus(Status status)
{
  mStatus = status;
  if (mCallback)
      (mCallback)(mStatus);
}

}
