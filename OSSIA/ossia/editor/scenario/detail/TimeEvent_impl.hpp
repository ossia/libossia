/*!
 * \file TimeEvent_impl.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <ossia/editor/expression/expression.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/state/state.hpp>

#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include "TimeNode_impl.hpp"
#include "Scenario_impl.hpp"

using namespace ossia;

namespace impl
{
class JamomaTimeEvent final : public time_event, public std::enable_shared_from_this<JamomaTimeEvent>
{

private:

# pragma mark -
# pragma mark Implementation specific

  time_event::ExecutionCallback mCallback;

  std::shared_ptr<time_node> mTimeNode;
  State mState;
  Status mStatus;

  expression_ptr mExpression;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaTimeEvent(time_event::ExecutionCallback,
                  std::shared_ptr<time_node> aTimeNode/* = nullptr*/,
                  expression_ptr anExpression /*= ExpressionTrue()*/);

  ~JamomaTimeEvent();

# pragma mark -
# pragma mark Execution

  void setCallback(time_event::ExecutionCallback) override;

  void happen() override;

  void dispose() override;

# pragma mark -
# pragma mark Edition

  void addState(State&&) override;

  void removeState(const State&) override;

# pragma mark -
# pragma mark Accessors

  const std::shared_ptr<time_node> & getTimeNode() const override;

  const State& getState() const override;

  const expression_ptr & getExpression() const override;

  time_event & setExpression(expression_ptr) override;

  Status getStatus() const override;

# pragma mark -
# pragma mark Implementation specific

  /* edit status and call ExecutionCallback
   \param #Status new status */
  void setStatus(Status);
};
}
