#pragma once
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/dataflow/graph_node.hpp>

namespace ossia
{

struct node_chain_process final :
    public ossia::time_process
{
    node_chain_process()
    {
      m_lastDate = ossia::Zero;
    }

    void state(ossia::time_value parent_date, double relative_position, ossia::time_value tick_offset, double speed) override
    {
      const ossia::token_request tk{parent_date, relative_position, tick_offset, speed};
      for(auto& node : nodes)
      {
        node->requested_tokens.push_back(tk);
      }
      m_lastDate = parent_date;
    }

    void add_node(std::shared_ptr<ossia::graph_node> n)
    {
      n->set_prev_date(this->m_lastDate);
      nodes.push_back(std::move(n));
    }

    void stop() override
    {
      for(auto& node : nodes)
      {
        node->all_notes_off();
      }
    }

    void offset(time_value date, double pos) override
    {
      if(node)
        node->set_prev_date(date);
      for(auto& node : nodes)
      {
        node->set_prev_date(date);
        node->all_notes_off();
      }
    }

    void transport(ossia::time_value date, double pos) override
    {
      if(node)
        node->set_prev_date(date);
      for(auto& node : nodes)
      {
        node->set_prev_date(date);
        node->all_notes_off();
      }
    }

    void mute_impl(bool b) override
    {
      for(auto& node : nodes)
        node->set_mute(b);
    }
    std::vector<std::shared_ptr<ossia::graph_node>> nodes;
    ossia::time_value m_lastDate{ossia::Infinite};
};

}
