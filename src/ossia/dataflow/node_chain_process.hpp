#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/scenario/time_process.hpp>

namespace ossia
{

struct node_chain_process final : public looping_process<node_chain_process>
{
  node_chain_process()
  {
  }

  void state_impl(const ossia::token_request& req)
  {
    for (auto& node : nodes)
    {
      node->request(req);
    }
  }

  void add_node(int64_t idx, std::shared_ptr<ossia::graph_node> n)
  {
    // n->set_prev_date(this->m_lastDate);
    nodes.insert(nodes.begin() + idx, std::move(n));
  }

  void stop() override
  {
    for (auto& node : nodes)
    {
      node->all_notes_off();
    }
  }

  void offset_impl(time_value date) override
  {
    for (auto& node : nodes)
    {
      node->all_notes_off();
    }
  }

  void transport_impl(ossia::time_value date) override
  {
    for (auto& node : nodes)
    {
      node->all_notes_off();
    }
  }

  void mute_impl(bool b) override
  {
    for (auto& node : nodes)
      node->set_mute(b);
  }
  std::vector<std::shared_ptr<ossia::graph_node>> nodes;
};
}
