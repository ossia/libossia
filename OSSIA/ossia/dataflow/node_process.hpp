#pragma once
#include <ossia/dataflow/graph.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{

class node_process : public ossia::time_process
{
public:
  node_process(
      std::shared_ptr<ossia::graph> g, ossia::node_ptr n)
      : m_graph{std::move(g)}
  {
    node = std::move(n);
  }
  node_process(
      std::shared_ptr<ossia::graph> g)
    : m_graph{std::move(g)}
  {
  }

  ossia::state_element offset(ossia::time_value, double pos) override
  {
    return {};
  }

  void set_node(std::shared_ptr<ossia::graph_node> n)
  {
    node = std::move(n);
  }

  ossia::state_element
  state(ossia::time_value parent_date, double relative_position, ossia::time_value tick_offset) override
  {
    if(node)
    {
      node->set_date(parent_date, relative_position);
      node->set_tick_offset(tick_offset);
      m_graph.lock()->enable(*node);
    }
    return {};
  }

  void start(ossia::state& st) override
  {
    // TODO reset all delay buffer positions
    if(node)
    {
      m_graph.lock()->enable(*node);
    }
  }

  void stop() override
  {
    if(node)
    {
      m_graph.lock()->disable(*node);
    }
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

private:
  std::weak_ptr<ossia::graph> m_graph;
};
}
