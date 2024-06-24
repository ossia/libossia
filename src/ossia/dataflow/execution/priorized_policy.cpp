#include "priorized_policy.hpp"

#include <ossia/dataflow/execution/to_state_element.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>
#include <ossia/network/base/node_attributes.hpp>

namespace ossia
{

void priorized_execution_state_policy::commit()
{
  // Here we use the priority of each node
  ossia::flat_map<
      std::tuple<ossia::net::priority, int64_t, int>, std::vector<ossia::state_element>>
      m_priorizedMessagesCache;
  for(auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    auto& [param, vec] = *it;
    m_commitOrderedState.clear();
    m_commitOrderedState.reserve(vec.size());
    state_flatten_visitor<ossia::flat_vec_state, false, true> vis{m_commitOrderedState};

    int64_t cur_ts = 0; // timestamp
    int cur_ms = 0;     // message stamp
    int cur_prio = 0;
    if(const auto& p = ossia::net::get_priority(param->get_node()))
      cur_prio = *p;

    for(auto& val : vec)
    {
      cur_ms = std::max(cur_ms, val.second);
      cur_ts = std::max(cur_ts, val.first.timestamp);
      vis(to_state_element(*param, std::move(val.first)));
    }

    auto& idx = m_priorizedMessagesCache[std::make_tuple(cur_prio, cur_ts, cur_ms)];
    for(auto& e : m_commitOrderedState)
      idx.push_back(std::move(e));

    vec.clear();
  }

  for(auto& vec : m_priorizedMessagesCache)
  {
    for(auto& mess : vec.second)
      ossia::launch(mess);
    vec.second.clear();
  }

  commit_common();
}

}
