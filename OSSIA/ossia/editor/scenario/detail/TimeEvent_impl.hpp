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

#include <OSSIA/editor/expression/expression.hpp>
#include "Editor/TimeConstraint.hpp"
#include "Editor/TimeEvent.hpp"
#include "Editor/TimeNode.hpp"
#include "Editor/State.hpp"

#include "TimeConstraint_impl.hpp"
#include "TimeNode_impl.hpp"
#include "Scenario_impl.hpp"

using namespace OSSIA;

namespace impl
{
class JamomaTimeEvent final : public TimeEvent, public enable_shared_from_this<JamomaTimeEvent>
{

private:

# pragma mark -
# pragma mark Implementation specific

  TimeEvent::ExecutionCallback      mCallback;

  shared_ptr<TimeNode>              mTimeNode;
  State                             mState;
  Status                            mStatus;

  shared_ptr<Expression>            mExpression;

public:

# pragma mark -
# pragma mark Life cycle

  JamomaTimeEvent(TimeEvent::ExecutionCallback,
                  shared_ptr<TimeNode> aTimeNode = nullptr,
                  shared_ptr<Expression> anExpression = ExpressionTrue);

  ~JamomaTimeEvent();

# pragma mark -
# pragma mark Execution

  void setCallback(TimeEvent::ExecutionCallback) override;

  void happen() override;

  void dispose() override;

# pragma mark -
# pragma mark Edition

  void addState(State&&) override;

  void removeState(const State&) override;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<TimeNode> & getTimeNode() const override;

  const State& getState() const override;

  const shared_ptr<Expression> & getExpression() const override;

  TimeEvent & setExpression(std::shared_ptr<Expression>) override;

  Status getStatus() const override;

# pragma mark -
# pragma mark Implementation specific

  /* edit status and call ExecutionCallback
   \param #Status new status */
  void setStatus(Status);
};
}
