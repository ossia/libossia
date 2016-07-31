#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
namespace ossia
{
std::shared_ptr<time_constraint> time_constraint::create(time_constraint::ExecutionCallback callback,
                                                         std::shared_ptr<time_event> startEvent,
                                                         std::shared_ptr<time_event> endEvent,
                                                         time_value nominal,
                                                         time_value min,
                                                         time_value max)
{
  auto timeConstraint = std::make_shared<detail::time_constraint_impl>(callback, startEvent, endEvent, nominal, min, max);

  // store the TimeConstraint into the start event as a next constraint
  if (std::find(startEvent->nextTimeConstraints().begin(),
                startEvent->nextTimeConstraints().end(),
                timeConstraint) == startEvent->nextTimeConstraints().end())
  {
    startEvent->nextTimeConstraints().push_back(timeConstraint);
  }

  // store the TimeConstraint into the end event as a previous constraint
  if (std::find(endEvent->previousTimeConstraints().begin(),
                endEvent->previousTimeConstraints().end(),
                timeConstraint) == endEvent->previousTimeConstraints().end())
  {
    endEvent->previousTimeConstraints().push_back(timeConstraint);
  }

  return timeConstraint;
}

time_constraint::~time_constraint() = default;
}
