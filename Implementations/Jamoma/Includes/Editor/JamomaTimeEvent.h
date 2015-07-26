/*!
 * \file JamomaTimeEvent.h
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

#include "JamomaTimeConstraint.h"
#include "JamomaTimeNode.h"
#include "JamomaScenario.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeEvent : public TimeEvent
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  TimeEvent::ExecutionCallback  mCallback;
  
  shared_ptr<TimeNode>          mTimeNode;
  shared_ptr<State>             mState;
  shared_ptr<Expression>        mExpression;
  Status                        mStatus;

public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeEvent(TimeEvent::ExecutionCallback,
                  shared_ptr<TimeNode> aTimeNode = nullptr,
                  shared_ptr<Expression> anExpression = nullptr);
  
  ~JamomaTimeEvent();

# pragma mark -
# pragma mark Execution
  
  void happen() override;
  
  void dispose() override;
  
# pragma mark -
# pragma mark Edition
  
  void addState(const std::shared_ptr<State>) override;
  
  void removeState(const std::shared_ptr<State>) override;

# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<TimeNode> & getTimeNode() const override;
  
  const shared_ptr<State> & getState() const override;
  
  const shared_ptr<Expression> & getExpression() const override;
  
  Status getStatus() const override;
  
# pragma mark -
# pragma mark Implementation specific
  
  void process();
  
  void setStatus(Status);
};
