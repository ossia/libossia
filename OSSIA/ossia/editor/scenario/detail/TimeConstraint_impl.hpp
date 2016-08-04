#pragma once

#include <ossia/editor/scenario/clock.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/editor/state/state.hpp>

#include "Clock_impl.hpp"
#include "TimeEvent_impl.hpp"
#include "TimeNode_impl.hpp"
#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

using namespace ossia;

using namespace std::placeholders;

namespace ossia
{
namespace detail
{
class time_constraint_impl final
    : public time_constraint,
      public std::enable_shared_from_this<time_constraint_impl>
{

private:
  time_constraint::ExecutionCallback mCallback;

  std::shared_ptr<time_event> mStartEvent;
  std::shared_ptr<time_event> mEndEvent;

  time_value mDurationNominal{};
  time_value mDurationMin{};
  time_value mDurationMax{};

public:
  time_constraint_impl(
      time_constraint::ExecutionCallback, std::shared_ptr<time_event>,
      std::shared_ptr<time_event>, time_value = Infinite, time_value = 0.,
      time_value = Infinite);

  ~time_constraint_impl();
private:
  void start() final override;

  void stop() final override;

  ossia::state offset(time_value) override;

  ossia::state state() override;

  void pause() final override;

  void resume() final override;

  void setCallback(ExecutionCallback) override;

  const time_value& getDurationNominal() const override;

  time_constraint& setDurationNominal(time_value) override;

  const time_value& getDurationMin() const override;

  time_constraint& setDurationMin(time_value) override;

  const time_value& getDurationMax() const override;

  time_constraint& setDurationMax(time_value) override;

  const std::shared_ptr<time_event>& getStartEvent() const override;

  const std::shared_ptr<time_event>& getEndEvent() const override;

  void addTimeProcess(std::shared_ptr<time_process>) override;

  void removeTimeProcess(std::shared_ptr<time_process>) override;

  void ClockCallback(
      time_value position, time_value date, unsigned char droppedTicks);
  ossia::state make_state();
};
}
}
