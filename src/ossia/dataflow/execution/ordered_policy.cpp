#include "ordered_policy.hpp"

#include <ossia/dataflow/execution/to_state_element.hpp>
#include <ossia/editor/state/detail/state_flatten_visitor.hpp>

namespace ossia
{

void ordered_execution_state_policy::commit()
{
  // TODO same for midi
  // m_flatMessagesCache.reserve(m_valueState.size());
  for(auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    auto& [param, vec] = *it;
    m_commitOrderedState.clear();
    m_commitOrderedState.reserve(vec.size());
    state_flatten_visitor<ossia::flat_vec_state, false, true> vis{m_commitOrderedState};

    int64_t cur_ts = 0; // timestamp
    int cur_ms = 0;     // message stamp
    for(auto& val : vec)
    {
      cur_ms = std::max(cur_ms, val.second);
      cur_ts = std::max(cur_ts, val.first.timestamp);
      vis(to_state_element(*it->first, std::move(val.first)));
    }

    auto& idx = m_flatMessagesCache[std::make_pair(cur_ts, cur_ms)];
    for(auto& e : m_commitOrderedState)
      idx.push_back(std::move(e));

    vec.clear();
  }

  for(auto& vec : m_flatMessagesCache)
  {
    for(auto& mess : vec.second)
      ossia::launch(mess);
    vec.second.clear();
  }

  commit_common();
}

}
