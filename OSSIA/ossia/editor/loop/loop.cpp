#include <ossia/editor/loop/detail/Loop_impl.hpp>

namespace ossia
{
  std::shared_ptr<loop> loop::create(
          time_value patternDuration,
          time_constraint::ExecutionCallback patternConstraintCallback,
          time_event::ExecutionCallback patternStartEventCallback,
          time_event::ExecutionCallback patternEndEventCallback)
  {
    return std::make_shared<detail::loop_impl>(
                patternDuration,
                std::move(patternConstraintCallback),
                std::move(patternStartEventCallback),
                std::move(patternEndEventCallback));
  }

  loop::~loop() = default;
}
