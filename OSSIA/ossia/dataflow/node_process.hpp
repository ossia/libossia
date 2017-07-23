#pragma once
#include <ossia/dataflow/graph.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{

class node_process : public ossia::time_process
{
public:
  node_process(
      std::shared_ptr<ossia::graph> g, std::shared_ptr<ossia::graph_node> n)
      : m_graph{std::move(g)}, m_node{std::move(n)}
  {
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
    m_node = std::move(n);
  }

  ossia::state_element
  state(ossia::time_value parent_date, double relative_position) override
  {
    if(m_node)
    {
      m_node->set_date(parent_date, relative_position);
      m_graph->enable(*m_node);
    }
    return {};
  }

  void start() override
  {
    // TODO reset all delay buffer positions
    if(m_node)
    {
      m_graph->enable(*m_node);
    }
  }

  void stop() override
  {
    if(m_node)
    {
      m_graph->disable(*m_node);
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
  std::shared_ptr<ossia::graph> m_graph;
  std::shared_ptr<ossia::graph_node> m_node;
};
}
