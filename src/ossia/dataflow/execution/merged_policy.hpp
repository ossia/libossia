#pragma once
#include <ossia/dataflow/execution/execution_policy.hpp>

#include <blockingconcurrentqueue.h>
namespace ossia
{
struct merged_execution_state_policy : local_state_execution_policy
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

  std::atomic_flag m_stopFlag = ATOMIC_FLAG_INIT;
  std::binary_semaphore m_execWait{0};

  std::vector<ossia::state_element> m_calice;

  int current_commit{};
};
}
