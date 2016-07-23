#include "Editor/TimeConstraint_impl.h"
#include <cassert>
#include <algorithm>
#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<TimeConstraint> TimeConstraint::create(TimeConstraint::ExecutionCallback callback,
                                                    shared_ptr<TimeEvent> startEvent,
                                                    shared_ptr<TimeEvent> endEvent,
                                                    TimeValue nominal,
                                                    TimeValue min,
                                                    TimeValue max)
  {
    auto timeConstraint = make_shared<impl::JamomaTimeConstraint>(callback, startEvent, endEvent, nominal, min, max);

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

  TimeConstraint::~TimeConstraint()
  {}
}
