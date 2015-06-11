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
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaTimeConstraint : public TimeConstraint
{

private:
  
  // Implementation specific
  TimeValue             mDuration;
  TimeValue             mDurationMin;
  TimeValue             mDurationMax;
  
  shared_ptr<TimeNode>  mStartNode;
  shared_ptr<TimeNode>  mEndNode;
  shared_ptr<State>     mStartState;
  shared_ptr<State>     mEndState;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaTimeConstraint(TimeValue nominal, TimeValue min /*= nominal*/, TimeValue max /*= nominal*/);
  
  JamomaTimeConstraint(const JamomaTimeConstraint * other);
  
  virtual ~JamomaTimeConstraint();
  
  shared_ptr<TimeConstraint> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Accessors
  
  const shared_ptr<TimeNode> & getStartNode() const override;
  
  void setStartNode(std::shared_ptr<TimeNode> startNode);
  
  const shared_ptr<TimeNode> & getEndNode() const override;
  
  void setEndNode(std::shared_ptr<TimeNode> endNode);
  
  const shared_ptr<State> & getStartState() const override;
  
  void setStartState(shared_ptr<State> startState) override;
  
  const shared_ptr<State> & getEndState() const override;
  
  void setEndState(shared_ptr<State> endState) override;
};