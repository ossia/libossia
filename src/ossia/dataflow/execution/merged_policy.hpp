#pragma once
#include <ossia/dataflow/execution/execution_policy.hpp>

#include <blockingconcurrentqueue.h>

#include <semaphore>

namespace ossia
{
struct
#if defined(OSSIA_TESTING)
    OSSIA_EXPORT
#endif
        merged_execution_state_policy : local_state_execution_policy
{
  void commit() override;
  ossia::mono_state m_monoState;
};

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
}
