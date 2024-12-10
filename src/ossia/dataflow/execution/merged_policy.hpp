#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution/local_state_execution_policy.hpp>
#include <ossia/editor/state/flat_vec_state.hpp>

#if !defined(OSSIA_FREESTANDING)
#include <blockingconcurrentqueue.h>
#endif

namespace ossia
{
struct OSSIA_TEST_EXPORT merged_execution_state_policy : local_state_execution_policy
{
  void commit() override;
  ossia::mono_state m_monoState;
};

#if !defined(OSSIA_FREESTANDING)
struct threaded_merged_execution_state_policy : local_state_execution_policy
{
  threaded_merged_execution_state_policy();
  ~threaded_merged_execution_state_policy();

  void commit() override;
  ossia::mono_state m_monoState;

  std::thread m_valuesOutputThread;

  // FIXME make sure that the sequencing is preserved so
  // that we always get all the messages for commit N before all the messages for commit N+1
  // FIXME in single thread mode we only need SPSC
  moodycamel::BlockingConcurrentQueue<std::vector<ossia::state_element>>
      m_messagesToApply;

  std::atomic_bool m_stopFlag{};

  std::vector<ossia::state_element> m_states;

  int current_commit{};
};
#endif
}
