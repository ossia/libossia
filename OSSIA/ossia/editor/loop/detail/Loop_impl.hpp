#pragma once

#include <ossia/editor/loop/loop.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

#include <cmath>

using namespace ossia;

using namespace std::placeholders;

namespace detail
{
class loop_impl final : public loop, public virtual time_process_impl
{
  private:
    std::shared_ptr<time_node>              mPatternStartNode;
    time_event::ExecutionCallback      mPatternStartEventCallback;

    std::shared_ptr<time_node>              mPatternEndNode;
    time_event::ExecutionCallback      mPatternEndEventCallback;

    std::shared_ptr<time_constraint>        mPatternConstraint;
    time_constraint::ExecutionCallback mPatternConstraintCallback;

    ossia::state                             mCurrentState;      // an internal State to return on state call
    ossia::state                             mOffsetState;       // an internal State built when offset is called

  public:
    loop_impl(time_value,
              time_constraint::ExecutionCallback,
              time_event::ExecutionCallback,
              time_event::ExecutionCallback);
    ~loop_impl();

  private:
    state_element offset(time_value) override;

    state_element state() override;

    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;

    const std::shared_ptr<time_constraint> getPatternTimeConstraint() const override;

    const std::shared_ptr<time_node> getPatternStartTimeNode() const override;

    const std::shared_ptr<time_node> getPatternEndTimeNode() const override;

  private:
    void PatternConstraintCallback(
        time_value,
        time_value,
        const ossia::state&);

    void PatternStartEventCallback(time_event::Status);

    void PatternEndEventCallback(time_event::Status);
};
}
