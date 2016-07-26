#include "Editor/Loop_impl.hpp"
#include <Misc/Util.h>

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Loop> Loop::create(
          TimeValue patternDuration,
          TimeConstraint::ExecutionCallback patternConstraintCallback,
          TimeEvent::ExecutionCallback patternStartEventCallback,
          TimeEvent::ExecutionCallback patternEndEventCallback)
  {
    return make_shared<impl::JamomaLoop>(
                patternDuration,
                std::move(patternConstraintCallback),
                std::move(patternStartEventCallback),
                std::move(patternEndEventCallback));
  }


  Loop::~Loop() = default;
}
