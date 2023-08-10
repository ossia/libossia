#pragma once

#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#include <ossia/dataflow/timed_value.hpp>
#include <ossia/dataflow/typed_value.hpp>
#include <ossia/dataflow/value_vector.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/detail/mutex.hpp>
#include <ossia/editor/state/flat_vec_state.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/protocols/midi/midi_device.hpp>
#include <ossia/protocols/midi/midi_protocol.hpp>
#if defined(OSSIA_SMALL_VECTOR)
#include <libremidi/message.hpp>
#endif

#include <cstdint>
#if SIZE_MAX == 0xFFFFFFFF // 32-bit
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#include <ossia/dataflow/value_port.hpp>
#endif

#include <ossia/detail/audio_spin_mutex.hpp>

#include <smallfun.hpp>

#if defined(OSSIA_PARALLEL)
using ossia_audio_lock_t = std::unique_lock<ossia::audio_spin_mutex>;
#define OSSIA_EXEC_STATE_LOCK_READ(state) \
  ossia_audio_lock_t ossia_read_lock      \
  {                                       \
    (state).mutex                         \
  }
#define OSSIA_EXEC_STATE_LOCK_WRITE(state) \
  ossia_audio_lock_t ossia_write_lock      \
  {                                        \
    (state).mutex                          \
  }
#else
using ossia_audio_lock_t = int;
#define OSSIA_EXEC_STATE_LOCK_READ(state) \
  ossia_audio_lock_t ossia_read_lock;     \
  (void)ossia_read_lock;
#define OSSIA_EXEC_STATE_LOCK_WRITE(state) \
  ossia_audio_lock_t ossia_write_lock;     \
  (void)ossia_write_lock;
#endif
namespace ossia
{

struct execution_state_policy
{
  execution_state_policy();
  virtual ~execution_state_policy();

  virtual void commit() = 0;

  virtual void clear_local_state() = 0;
  //virtual void reset() = 0;

  virtual bool find_and_copy(net::parameter_base& addr, inlet& in) = 0;
  virtual bool in_local_scope(net::parameter_base& other) const = 0;

  virtual void insert(ossia::net::parameter_base& dest, const ossia::value_port& v) = 0;
  virtual void insert(ossia::audio_parameter& dest, const audio_port& v) = 0;
  virtual void insert(ossia::net::midi::midi_parameter& dest, const midi_port& v) = 0;
};

struct local_state_execution_policy : execution_state_policy
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
  ossia::hash_map<ossia::net::parameter_base*, value_vector<libremidi::message>>
      m_midiState;

  TS_GUARDED_BY(mutex)
  int m_msgIndex{};
};

inline ossia::message
to_state_element(ossia::net::parameter_base& p, ossia::typed_value&& v)
{
  ossia::message m{p, std::move(v.value)};
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
  return m;
}

inline ossia::message
to_state_element(ossia::net::parameter_base& p, const ossia::typed_value& v)
{
  ossia::message m{p, v.value};
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
  return m;
}

}
