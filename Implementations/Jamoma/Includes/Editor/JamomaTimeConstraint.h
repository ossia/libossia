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

#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{

private:
  
  // Implementation specific
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
  
  const TimeValue & getDurationMin() const override;

  const TimeValue & getDurationMax() const override;
  
  const shared_ptr<TimeEvent> & getStartEvent() const override;
  
  const shared_ptr<TimeEvent> & getEndEvent() const override;
};