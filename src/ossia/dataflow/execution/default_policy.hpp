#pragma once
#include <ossia/dataflow/execution/execution_policy.hpp>

namespace ossia
{
struct default_execution_state_policy : local_state_execution_policy
{
  void commit() override;
  ossia::flat_vec_state m_commitOrderedState;
};
}
