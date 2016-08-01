#pragma once

#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/state/state.hpp>

#include "Scenario_impl.hpp"
#include "TimeNode_impl.hpp"
#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>

using namespace ossia;

namespace ossia
{
namespace detail
{
class time_event_impl final
    : public time_event,
      public std::enable_shared_from_this<time_event_impl>
{
private:
  time_event::ExecutionCallback mCallback;

  std::shared_ptr<time_node> mTimeNode;
  state mState;
  Status mStatus;

  expression_ptr mExpression;

public:
  time_event_impl(
      time_event::ExecutionCallback, std::shared_ptr<time_node> aTimeNode,
      expression_ptr anExpression);

  ~time_event_impl();

  /* edit status and call ExecutionCallback
   \param #Status new status */
  void setStatus(Status);

private:
  void setCallback(time_event::ExecutionCallback) override;

  void happen() override;

  void dispose() override;

  void addState(state&&) override;

  void removeState(const state&) override;

  const std::shared_ptr<time_node>& getTimeNode() const override;

  const state& getState() const override;

  const expression& getExpression() const override;

  time_event& setExpression(expression_ptr) override;

  Status getStatus() const override;

};
}
}
