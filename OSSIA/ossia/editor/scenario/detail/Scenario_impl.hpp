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

#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include "Clock_impl.hpp"
#include "Scenario_impl.hpp"
#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include "TimeEvent_impl.hpp"
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>
#include "TimeNode_impl.hpp"

#include <list>
#include <utility>

using namespace ossia;

namespace impl
{
class JamomaScenario final : public scenario, public JamomaTimeProcess
{

private:

# pragma mark -
# pragma mark Implementation specific

  ptr_container<time_constraint>   mTimeContraints;
  ptr_container<time_node>         mTimeNodes;         // list of all TimeNodes of the scenario (the first is the start node, the second is the end node)

  std::list<std::pair<time_value, std::shared_ptr<time_event>>> mPastEventList; // a temporary list to order all past events to build the offset state

public:

# pragma mark -
# pragma mark Life cycle

  JamomaScenario();

  ~JamomaScenario();

# pragma mark -
# pragma mark Execution

  StateElement offset(time_value) override;

  StateElement state() override;

# pragma mark -
# pragma mark Execution - Implementation specific

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

# pragma mark -
# pragma mark Edition

  void addTimeConstraint(std::shared_ptr<time_constraint>) override;

  void removeTimeConstraint(const std::shared_ptr<time_constraint>&) override;

  void addTimeNode(std::shared_ptr<time_node>) override;

  void removeTimeNode(const std::shared_ptr<time_node>&) override;

# pragma mark -
# pragma mark Accessors

  const std::shared_ptr<time_node> & getStartTimeNode() const override;

# pragma mark -
# pragma mark TimeNodes and TimeConstraints

  const ptr_container<time_node>& timeNodes() const override;

  const ptr_container<time_constraint>& timeConstraints() const override;

# pragma mark -
# pragma mark Implementation specific

  /* order all HAPPENED TimeEvents into mOffetEventMap */
  void process_offset(std::shared_ptr<time_node>, time_value);
};
}
