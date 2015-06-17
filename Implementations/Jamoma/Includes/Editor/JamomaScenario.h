/*!
 * \file JamomaScenario.h
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
#include "Editor/Scenario.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"

using namespace OSSIA;
using namespace std;

class JamomaScenario : public Scenario
{

private:
  
  // Implementation specific
  shared_ptr<TimeConstraint>  mParentConstraint;
  
  Container<TimeConstraint>   mTimeContraints;
  Container<TimeNode>         mTimeNodes;         // list of all time nodes of the scenario (the first is the start node, the second is the end node)
  
  bool                        mIsKiller;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaScenario(shared_ptr<TimeConstraint> = nullptr);
  
  JamomaScenario(const JamomaScenario * other);
  
  ~JamomaScenario();
  
  shared_ptr<Scenario> clone() const override;

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Edition
  
  void addConstraint(const shared_ptr<TimeConstraint> constraint) override;
  
  void removeConstraint(const shared_ptr<TimeConstraint> constraint) override;
  
  void addTimeNode(const shared_ptr<TimeNode>) override;

  void removeTimeNode(const shared_ptr<TimeNode>) override;

# pragma mark -
# pragma mark Accessors
  
  bool isKiller() const override;
  
  void setKiller(bool isKiller) override;
  
  const shared_ptr<TimeNode> & getStartNode() const override;
  
  const shared_ptr<TimeNode> & getEndNode() const override;
  
  const shared_ptr<TimeConstraint> & getParentTimeConstraint() const override;
  
  const shared_ptr<State> & getStartState() const override;
  
  const shared_ptr<State> & getEndState() const override;
};
