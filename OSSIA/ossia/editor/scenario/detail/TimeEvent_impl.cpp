#include "TimeEvent_impl.hpp"
#include <ossia/detail/algorithms.hpp>
#include <ossia/editor/state/state_element.hpp>
namespace detail
{
time_event_impl::time_event_impl(time_event::ExecutionCallback callback,
                                 std::shared_ptr<time_node> aTimeNode,
                                 expression_ptr anExpression) :
mCallback(callback),
mTimeNode(aTimeNode),
mStatus(time_event::Status::NONE),
mExpression(std::move(anExpression))
{
}

time_event_impl::~time_event_impl() = default;

void time_event_impl::setCallback(time_event::ExecutionCallback callback)
{
  mCallback = callback;
}

void time_event_impl::happen()
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

void time_event_impl::dispose()
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

void time_event_impl::addState(state&& state)
{
    mState.add(state_element{std::move(state)});
}

void time_event_impl::removeState(const state& state)
{
    mState.remove(state);
}

const std::shared_ptr<time_node> & time_event_impl::getTimeNode() const
{
  return mTimeNode;
}

const state& time_event_impl::getState() const
{
  return mState;
}

const expression & time_event_impl::getExpression() const
{
  return *mExpression;
}

time_event & time_event_impl::setExpression(expression_ptr exp)
{
  assert(exp);

  mExpression = std::move(exp);

  return *this;
}

time_event::Status time_event_impl::getStatus() const
{
  return mStatus;
}

void time_event_impl::setStatus(Status status)
{
  mStatus = status;
  if (mCallback)
      (mCallback)(mStatus);
}

}
