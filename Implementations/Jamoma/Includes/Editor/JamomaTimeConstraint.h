/*!
 * \file JamomaTimeConstraint.h
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

#include "Editor/Clock.h"
#include "Editor/State.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"

#include "JamomaClock.h"
#include "JamomaTimeConstraint.h"
#include "JamomaTimeEvent.h"
#include "JamomaTimeNode.h"
#include "JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaTimeConstraint : public TimeConstraint, public JamomaClock, public enable_shared_from_this<JamomaTimeConstraint>
{

private:
  
# pragma mark -
# pragma mark Implementation specific
  
  TimeConstraint::ExecutionCallback   mCallback;
  
  shared_ptr<State>                   mCurrentState;    // an internal State to update at each tick of the clock
  
  shared_ptr<TimeEvent>               mStartEvent;
  shared_ptr<TimeEvent>               mEndEvent;
  
  TimeValue                           mDurationMin;
  TimeValue                           mDurationMax;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeConstraint(TimeConstraint::ExecutionCallback,
                       shared_ptr<TimeEvent>,
                       shared_ptr<TimeEvent>,
                       const TimeValue& = Infinite,
                       const TimeValue& = 0.,
                       const TimeValue& = Infinite);
  
  JamomaTimeConstraint(const JamomaTimeConstraint *);
  
  shared_ptr<TimeConstraint> clone() const override;
  
  ~JamomaTimeConstraint();

# pragma mark -
# pragma mark Execution
  
  shared_ptr<StateElement> state(const TimeValue&, const TimeValue&) override;

# pragma mark -
# pragma mark Accessors
  
  const TimeValue & getDurationMin() const override;
  
  TimeConstraint & setDurationMin(const TimeValue&) override;

  const TimeValue & getDurationMax() const override;
  
  TimeConstraint & setDurationMax(const TimeValue&) override;
  
  const shared_ptr<TimeEvent> & getStartEvent() const override;
  
  const shared_ptr<TimeEvent> & getEndEvent() const override;
  
# pragma mark -
# pragma mark TimeProcesses
  
  void addTimeProcess(std::shared_ptr<TimeProcess>) override;

  void removeTimeProcess(std::shared_ptr<TimeProcess>) override;

# pragma mark -
# pragma mark Implementation specific
  
  void process();

private:
  
  void ClockCallback(const TimeValue& position, const TimeValue& date, unsigned char droppedTicks);
};