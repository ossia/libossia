#include "TimeEvent_impl.h"
#include <Misc/Util.h>
#include <Editor/StateElement.h>
namespace impl
{

# pragma mark -
# pragma mark Life cycle

JamomaTimeEvent::JamomaTimeEvent(TimeEvent::ExecutionCallback callback,
                                 shared_ptr<TimeNode> aTimeNode,
                                 shared_ptr<Expression> anExpression) :
mCallback(callback),
mTimeNode(aTimeNode),
mStatus(TimeEvent::Status::NONE),
mExpression(anExpression)
{
}

JamomaTimeEvent::~JamomaTimeEvent()
{}

# pragma mark -
# pragma mark Execution

void JamomaTimeEvent::setCallback(TimeEvent::ExecutionCallback callback)
{
  mCallback = callback;
}

void JamomaTimeEvent::happen()
{
  if (mStatus != TimeEvent::Status::PENDING)
    throw runtime_error("only PENDING event can happens");

  mStatus = TimeEvent::Status::HAPPENED;

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
  if (mStatus == TimeEvent::Status::HAPPENED)
    throw runtime_error("HAPPENED event cannot be disposed");

  mStatus = TimeEvent::Status::DISPOSED;

  // dispose next TimeConstraints end event if everything is disposed before
  for (auto& nextTimeConstraint : nextTimeConstraints())
  {
    bool dispose = true;

    for (auto& previousTimeConstraint : nextTimeConstraint->getEndEvent()->previousTimeConstraints())
    {
      if (previousTimeConstraint->getStartEvent()->getStatus() != TimeEvent::Status::DISPOSED)
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
  // store a state if it is not already stored
  auto& se = mState.children;
  if (std::find(se.begin(), se.end(), state) == se.end())
  {
    se.push_back(StateElement{std::move(state)});
  }
}

void JamomaTimeEvent::removeState(const State& state)
{
    auto& se = mState.children;
  auto it = std::find(se.begin(), se.end(), state);
  if (it != se.end())
  {
      se.erase(it);
  }
}

# pragma mark -
# pragma mark Accessors

const shared_ptr<TimeNode> & JamomaTimeEvent::getTimeNode() const
{
  return mTimeNode;
}

const State& JamomaTimeEvent::getState() const
{
  return mState;
}

const shared_ptr<Expression> & JamomaTimeEvent::getExpression() const
{
  return mExpression;
}

TimeEvent & JamomaTimeEvent::setExpression(std::shared_ptr<Expression> expression)
{
  assert(expression != nullptr);

  mExpression = std::move(expression);

  return *this;
}

TimeEvent::Status JamomaTimeEvent::getStatus() const
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
