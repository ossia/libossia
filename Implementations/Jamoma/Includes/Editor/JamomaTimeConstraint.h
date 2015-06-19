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
#include "Editor/TimeNode.h"
#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{

private:
  
  // Implementation specific
  shared_ptr<TimeNode>    mStartNode;
  shared_ptr<TimeNode>    mEndNode;
  
  TimeValue               mDuration;
  TimeValue               mDurationMin;
  TimeValue               mDurationMax;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeConstraint(shared_ptr<TimeNode>,
                       shared_ptr<TimeNode>,
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
  
  const shared_ptr<TimeNode> & getStartNode() const override;
  
  const shared_ptr<TimeNode> & getEndNode() const override;
};