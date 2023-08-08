#pragma once
#include <ossia/dataflow/execution/execution_policy.hpp>

namespace ossia
{
struct ordered_execution_state_policy : local_state_execution_policy
{
  void commit() override;
  ossia::flat_map<std::pair<int64_t, int>, std::vector<ossia::state_element>>
      m_flatMessagesCache;

  ossia::flat_vec_state m_commitOrderedState;
};
}
