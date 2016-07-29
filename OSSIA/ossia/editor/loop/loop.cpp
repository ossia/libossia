#include <ossia/editor/loop/detail/Loop_impl.hpp>
#include <ossia/detail/algorithms.hpp>

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life cycle

namespace ossia
{
  std::shared_ptr<loop> loop::create(
          time_value patternDuration,
          time_constraint::ExecutionCallback patternConstraintCallback,
          time_event::ExecutionCallback patternStartEventCallback,
          time_event::ExecutionCallback patternEndEventCallback)
  {
    return std::make_shared<impl::JamomaLoop>(
                patternDuration,
                std::move(patternConstraintCallback),
                std::move(patternStartEventCallback),
                std::move(patternEndEventCallback));
  }


  loop::~loop() = default;
}
