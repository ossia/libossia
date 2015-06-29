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

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{

private:
  
# pragma mark -
# pragma mark Implementation specific
  
  shared_ptr<TimeEvent>   mStartEvent;
  shared_ptr<TimeEvent>   mEndEvent;
  
  TimeValue               mDuration;
  TimeValue               mDurationMin;
  TimeValue               mDurationMax;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeConstraint(shared_ptr<TimeEvent>,
                       shared_ptr<TimeEvent>,
                       const TimeValue& = Infinite,
                       const TimeValue& = 0.,
                       const TimeValue& = Infinite);
  
  JamomaTimeConstraint(const JamomaTimeConstraint * other);
  
  ~JamomaTimeConstraint();
  
  shared_ptr<TimeConstraint> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Accessors
  
  const TimeValue & getDuration() const override;
  
  TimeConstraint & setDuration(const TimeValue&) override;
  
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
};