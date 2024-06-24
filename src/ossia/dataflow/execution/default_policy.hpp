#pragma once
#include <ossia/dataflow/execution/local_state_execution_policy.hpp>
#include <ossia/editor/state/flat_vec_state.hpp>

namespace ossia
{
struct default_execution_state_policy : local_state_execution_policy
{
  void commit() override;
  ossia::flat_vec_state m_commitOrderedState;
};
}
