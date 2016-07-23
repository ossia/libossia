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

#include "Editor/Expression.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeNode.h"
#include "Editor/State.h"

#include "TimeConstraint_impl.h"
#include "TimeNode_impl.h"
#include "Scenario_impl.h"

using namespace OSSIA;
using namespace std;
namespace impl
{
class JamomaTimeEvent final : public TimeEvent, public enable_shared_from_this<JamomaTimeEvent>
{

private:

# pragma mark -
# pragma mark Implementation specific

  TimeEvent::ExecutionCallback      mCallback;

  shared_ptr<TimeNode>              mTimeNode;
  shared_ptr<State>                 mState;
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

  void addState(std::shared_ptr<State>) override;

  void removeState(const std::shared_ptr<State>&) override;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<TimeNode> & getTimeNode() const override;

  const shared_ptr<State> & getState() const override;

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
