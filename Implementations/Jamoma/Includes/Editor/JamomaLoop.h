/*!
 * \file JamomaLoop.h
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

#include "Editor/Loop.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeValue.h"

#include "JamomaTimeConstraint.h"
#include "JamomaTimeProcess.h"

#include <cmath>

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaLoop : public Loop, public virtual JamomaTimeProcess
{
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<TimeNode>              mPatternStartNode;
  TimeEvent::ExecutionCallback      mPatternStartEventCallback;
  
  shared_ptr<TimeNode>              mPatternEndNode;
  TimeEvent::ExecutionCallback      mPatternEndEventCallback;
  
  shared_ptr<TimeConstraint>        mPatternConstraint;
  TimeConstraint::ExecutionCallback mPatternConstraintCallback;

  shared_ptr<State>                 mCurrentState;      // an internal State to return on state call
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaLoop(const TimeValue&,
             TimeConstraint::ExecutionCallback,
             TimeEvent::ExecutionCallback,
             TimeEvent::ExecutionCallback);
  
  JamomaLoop(const JamomaLoop *);
  
  shared_ptr<Loop> clone() const override;
  
  ~JamomaLoop();

# pragma mark -
# pragma mark Execution

  shared_ptr<StateElement> state(const TimeValue&, const TimeValue&) override;
  
# pragma mark -
# pragma mark Execution - Implementation specific
  
  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;
  
# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<TimeConstraint> getPatternTimeConstraint() const override;

  const shared_ptr<TimeNode> getPatternStartTimeNode() const override;
  
  const shared_ptr<TimeNode> getPatternEndTimeNode() const override;
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  /*! append each message of the state to the current state in order to eliminate address redundancy 
   \todo this code is also in JamomaScenario so we should find a way to factorise it */
  void flattenAndFilter(const shared_ptr<StateElement>);
  
  void PatternConstraintCallback(const TimeValue&, const TimeValue&, std::shared_ptr<StateElement>);
  
  void PatternStartEventCallback(TimeEvent::Status);
  
  void PatternEndEventCallback(TimeEvent::Status);
};
