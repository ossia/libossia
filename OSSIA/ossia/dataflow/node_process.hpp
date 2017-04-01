#pragma once
#include <ossia/dataflow/graph.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{

class graph_process : public ossia::time_process
{
public:
  graph_process(std::shared_ptr<ossia::graph> g, std::shared_ptr<ossia::graph_node> n):
    graph{g}, node{n} { }
  std::shared_ptr<ossia::graph> graph;
  std::shared_ptr<ossia::graph_node> node;

  ossia::state_element offset(ossia::time_value) override
  {
    return {};
  }

  ossia::state_element state() override
  {
    node->set_time(parent()->getDate() / parent()->getDurationNominal());
    graph->enable(*node);
    return {};
  }

  void start() override
  {
    // TODO reset all delay buffer positions
    graph->enable(*node);
  }

  void stop() override
  {
    graph->disable(*node);
  }

  void pause() override
  {
  }

  void resume() override
  {
  }

  void mute_impl(bool) override
  {
  }
};

}
