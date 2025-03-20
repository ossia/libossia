#pragma once
#include <ossia/dataflow/execution/execution_policy.hpp>
#include <ossia/dataflow/typed_value.hpp>
#include <ossia/dataflow/value_vector.hpp>
#include <ossia/detail/audio_spin_mutex.hpp>
#include <ossia/detail/hash_map.hpp>

#include <libremidi/ump.hpp>

namespace ossia
{

struct OSSIA_TEST_EXPORT local_state_execution_policy : execution_state_policy
{
  local_state_execution_policy();
  virtual ~local_state_execution_policy();

  bool find_and_copy(net::parameter_base& addr, inlet& in) override;
  void clear_local_state() override;

  void commit_common();

  bool in_local_scope(net::parameter_base& other) const override;

  void insert(ossia::net::parameter_base& dest, const ossia::value_port& v) override;
  void insert(ossia::audio_parameter& dest, const audio_port& v) override;
  void insert(ossia::net::midi::midi_parameter& dest, const midi_port& v) override;

  mutable ossia::audio_spin_mutex mutex;
  // private:// disabled due to tests, but for some reason can't make friend
  // work
  // using value_state_impl = ossia::flat_multimap<int64_t,
  // std::pair<ossia::value, int>>;

  TS_GUARDED_BY(mutex)
  ossia::hash_map<ossia::net::parameter_base*, value_vector<std::pair<typed_value, int>>>
      m_valueState;

  TS_GUARDED_BY(mutex)
  ossia::hash_map<ossia::audio_parameter*, audio_port> m_audioState;

  TS_GUARDED_BY(mutex)
  ossia::hash_map<ossia::net::parameter_base*, value_vector<libremidi::ump>> m_midiState;

  TS_GUARDED_BY(mutex)
  int m_msgIndex{};
};
}
