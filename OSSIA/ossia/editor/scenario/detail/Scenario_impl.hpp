#pragma once

#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_node.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include "Clock_impl.hpp"
#include "Scenario_impl.hpp"
#include "TimeEvent_impl.hpp"
#include "TimeNode_impl.hpp"
#include <ossia/editor/scenario/detail/TimeConstraint_impl.hpp>
#include <ossia/editor/scenario/detail/TimeProcess_impl.hpp>

#include <list>
#include <utility>

using namespace ossia;

namespace ossia
{
namespace detail
{
class scenario_impl final : public scenario
{

private:
  ptr_container<time_constraint> mTimeContraints;
  ptr_container<time_node> mTimeNodes; // list of all TimeNodes of the scenario
                                       // (the first is the start node, the
                                       // second is the end node)

  std::list<std::pair<time_value, std::shared_ptr<time_event>>>
      mPastEventList; // a temporary list to order all past events to build the
                      // offset state

public:
  scenario_impl();

  ~scenario_impl();

private:
  state_element offset(time_value) override;

  state_element state() override;

  void start() override;
  void stop() override;
  void pause() override;
  void resume() override;

  void addTimeConstraint(std::shared_ptr<time_constraint>) override;

  void removeTimeConstraint(const std::shared_ptr<time_constraint>&) override;

  void addTimeNode(std::shared_ptr<time_node>) override;

  void removeTimeNode(const std::shared_ptr<time_node>&) override;

  const std::shared_ptr<time_node>& getStartTimeNode() const override;

  const ptr_container<time_node>& timeNodes() const override;

  const ptr_container<time_constraint>& timeConstraints() const override;

  /* order all HAPPENED TimeEvents into mOffetEventMap */
  void process_offset(time_node&, time_value);
};
}
}
