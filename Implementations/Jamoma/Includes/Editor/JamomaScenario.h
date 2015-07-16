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

#include "Editor/Clock.h"
#include "Editor/Scenario.h"
#include "Editor/TimeConstraint.h"
#include "Editor/TimeEvent.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"

#include "JamomaTimeProcess.h"

using namespace OSSIA;
using namespace std;
using namespace std::placeholders;

class JamomaScenario : public Scenario, public JamomaTimeProcess
{

private:
  
# pragma mark -
# pragma mark Implementation specific

  Container<TimeConstraint>   mTimeContraints;
  Container<TimeNode>         mTimeNodes;         // list of all TimeNodes of the scenario (the first is the start node, the second is the end node)
  
  bool                        mKiller;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaScenario(TimeProcess::ExecutionCallback,
                 shared_ptr<State>,
                 shared_ptr<State>);
  
  JamomaScenario(const JamomaScenario *);
  
  shared_ptr<Scenario> clone() const override;
  
  ~JamomaScenario();

# pragma mark -
# pragma mark Execution
  
  void play(bool log = false, string name = "") const override;
  
  void stop() const override;
  
  void pause() const override;
  
  void resume() const override;
  
  shared_ptr<State> state() const override;
  
# pragma mark -
# pragma mark Edition
  
  void addTimeConstraint(const shared_ptr<TimeConstraint>) override;
  
  void removeTimeConstraint(const shared_ptr<TimeConstraint>) override;
  
  void addTimeNode(const shared_ptr<TimeNode>) override;

  void removeTimeNode(const shared_ptr<TimeNode>) override;

# pragma mark -
# pragma mark Accessors
  
  bool isKiller() const override;
  
  void setKiller(bool isKiller) override;
  
  const shared_ptr<TimeNode> & getStartNode() const override;
  
  const shared_ptr<TimeNode> & getEndNode() const override;
  
  const shared_ptr<State> & getStartState() const override;
  
  const shared_ptr<State> & getEndState() const override;
  
  const shared_ptr<Clock> & getClock() const override;
  
  const shared_ptr<TimeConstraint> & getParentTimeConstraint() const override;
  
private:
  
# pragma mark -
# pragma mark Implementation specific
  
  void ClockCallback(const TimeValue&, const TimeValue&);
};
