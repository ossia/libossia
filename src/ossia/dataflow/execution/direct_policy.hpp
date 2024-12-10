#pragma once
#include <ossia/detail/config.hpp>
#if !defined(OSSIA_FREESTANDING)
#include <ossia/dataflow/execution/execution_policy.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/network/base/bundle.hpp>

#include <blockingconcurrentqueue.h>

#include <thread>

namespace ossia
{
// If we do not use the local state, we can use this policy instead
struct direct_execution_state_policy : execution_state_policy
{
  direct_execution_state_policy();
  ~direct_execution_state_policy();

  void commit() override;

  void clear_local_state() override;

  bool find_and_copy(net::parameter_base& addr, inlet& in) override;
  bool in_local_scope(net::parameter_base& other) const override;

  void insert(ossia::net::parameter_base& dest, const ossia::value_port& v) override;
  void insert(ossia::audio_parameter& dest, const audio_port& v) override;
  void insert(ossia::net::midi::midi_parameter& dest, const midi_port& v) override;

  void process_messages();
  void process_midi();

  struct T : moodycamel::ConcurrentQueueDefaultTraits
  {
    static constexpr std::size_t CQ_BLOCK_SIZE = 2048;
  };

  moodycamel::BlockingConcurrentQueue<ossia::bundle_element, T> m_messagesToApply;

  static constexpr int max_threads = 16;
  struct net_thread
  {
    std::thread thread;
  } m_threads[max_threads];

  std::thread m_midiThread;

  std::atomic_bool m_startFlag{};
  std::atomic_bool m_stopFlag{};

  struct audio_msg
  {
    const ossia::audio_port* src{};
    ossia::audio_parameter* dst{};
  };
  ossia::mpmc_queue<audio_msg> m_audioQueue;

  struct midi_msg
  {
    const ossia::midi_port* src{};
    ossia::net::midi::midi_parameter* dst{};
  };
  ossia::mpmc_queue<midi_msg> m_midiQueue;
};
}
#endif
