#pragma once
#include <ossia/dataflow/graph.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_process.hpp>

namespace ossia
{

class node_process : public ossia::time_process
{
public:
  node_process(ossia::node_ptr n)
  {
    node = std::move(n);
  }
  node_process() = default;
  void offset(ossia::time_value, double pos) override
  {
  }

  void set_node(std::shared_ptr<ossia::graph_node> n)
  {
    node = std::move(n);
  }

  void
  state(ossia::time_value parent_date, double relative_position, ossia::time_value tick_offset) override
  {
    if(node)
    {
      node->requested_tokens.push_back({parent_date, relative_position, tick_offset});
    }
  }

  void start() override
  {
    // TODO reset all delay buffer positions
  }

  void stop() override
  {
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
