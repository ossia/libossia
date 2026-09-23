#pragma once
#include <ossia/detail/config.hpp>
#if SIZE_MAX == 0xFFFFFFFF // 32-bit
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#include <ossia/dataflow/value_port.hpp>
#endif

namespace ossia
{
namespace net
{
class parameter_base;
namespace midi
{
class midi_parameter;
}
}
class audio_parameter;
struct audio_port;
struct value_port;
struct midi_port;
struct inlet;
struct outlet;
struct OSSIA_TEST_EXPORT execution_state_policy
{
  execution_state_policy();
  execution_state_policy(const execution_state_policy&) = delete;
  execution_state_policy(execution_state_policy&&) = delete;
  execution_state_policy& operator=(const execution_state_policy&) = delete;
  execution_state_policy& operator=(execution_state_policy&&) = delete;

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
}
