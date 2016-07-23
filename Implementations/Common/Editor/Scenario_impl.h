/*!
 * \file Scenario_impl.h
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
#include "Editor/TimeEvent.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeProcess.h"
#include "Editor/TimeValue.h"

#include "Clock_impl.h"
#include "Scenario_impl.h"
#include "TimeConstraint_impl.h"
#include "TimeEvent_impl.h"
#include "TimeProcess_impl.h"
#include "TimeNode_impl.h"

#include <list>
#include <utility>

using namespace OSSIA;
using namespace std;
namespace impl
{
class JamomaScenario final : public Scenario, public JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  Container<TimeConstraint>   mTimeContraints;
  Container<TimeNode>         mTimeNodes;         // list of all TimeNodes of the scenario (the first is the start node, the second is the end node)

  State                       mCurrentState;      // an internal State to return on state call
  State                       mOffsetState;       // an internal State built when offset is called

  std::list<std::pair<TimeValue, shared_ptr<TimeEvent>>> mPastEventList; // a temporary list to order all past events to build the offset state

public:

# pragma mark -
# pragma mark Life cycle

  JamomaScenario();

  JamomaScenario(const JamomaScenario *);

  shared_ptr<Scenario> clone() const override;

  ~JamomaScenario();

# pragma mark -
# pragma mark Execution

  StateElement offset(TimeValue) override;

  StateElement state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Edition

  void addTimeConstraint(shared_ptr<TimeConstraint>) override;

  void removeTimeConstraint(const shared_ptr<TimeConstraint>&) override;

  void addTimeNode(shared_ptr<TimeNode>) override;

  void removeTimeNode(const shared_ptr<TimeNode>&) override;

# pragma mark -
# pragma mark Accessors

  const shared_ptr<TimeNode> & getStartTimeNode() const override;

# pragma mark -
# pragma mark TimeNodes and TimeConstraints

  const Container<TimeNode>& timeNodes() const override;

  const Container<TimeConstraint>& timeConstraints() const override;

# pragma mark -
# pragma mark Implementation specific

  /* order all HAPPENED TimeEvents into mOffetEventMap */
  void process_offset(shared_ptr<TimeNode>, TimeValue);
};
}
