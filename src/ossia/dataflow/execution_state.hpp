#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/value_vector.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <ossia/network/base/device.hpp>
#if defined(OSSIA_SMALL_VECTOR)
#include <libremidi/message.hpp>
#else
namespace libremidi
{
struct message;
}
#endif
namespace ossia
{
namespace net::midi
{
class midi_protocol;
class midi_parameter;
}
class state;
class message_queue;
class audio_parameter;
struct typed_value;
struct timed_value;
struct local_pull_visitor;
struct global_pull_visitor;
struct state_exec_visitor;
struct execution_state_policy;
struct OSSIA_EXPORT execution_state : public Nano::Observer
{
  execution_state();
  execution_state(const execution_state&) = delete;
  execution_state(execution_state&&) = delete;
  execution_state& operator=(const execution_state&) = delete;
  execution_state& operator=(execution_state&&) = delete;
  ~execution_state();

  void set_policy(const tick_setup_options&);

  /// To be called from the edition thread ///
  void register_device(ossia::net::device_base* d);
  void unregister_device(net::device_base* d);

  const ossia::small_vector<ossia::net::device_base*, 4>& edit_devices() const noexcept
  {
    return m_devices_edit;
  }

  /// To be called from the execution thread ///
  void register_port(const ossia::inlet& port);
  void register_port(const ossia::outlet& port);
  void unregister_port(const ossia::inlet& port);
  void unregister_port(const ossia::outlet& port);

  const ossia::small_vector<ossia::net::device_base*, 4>& exec_devices() const noexcept
  {
    return m_devices_exec;
  }

  ossia::net::node_base* find_node(std::string_view name) const noexcept
  {
    for(auto dev : m_devices_exec)
    {
      if(auto res = ossia::net::find_node(dev->get_root_node(), name))
        return res;
    }
    return nullptr;
  }

  template <typename T>
  auto get_value_or(std::string_view v, const T& val) noexcept
  {
    if(auto node = find_node(v))
      return ossia::convert<T>(node->get_parameter()->value());
    return val;
  }

  void begin_tick();

  void clear_devices();
  void reset();

  void advance_tick(std::size_t);
  void apply_device_changes();

  void find_and_copy(ossia::net::parameter_base& addr, inlet& in);
  void copy_from_global(ossia::net::parameter_base& addr, inlet& in);
  void copy_from_global_node(ossia::net::node_base& addr, inlet& in);

  void insert(ossia::net::parameter_base& dest, const ossia::value_port& v);
  void insert(ossia::audio_parameter& dest, const audio_port& v);
  void insert(ossia::net::midi::midi_parameter& dest, const midi_port& v);

  void commit();

  bool in_local_scope(ossia::net::parameter_base& other) const;

  int sampleRate{44100};
  int bufferSize{64};
  double modelToSamplesRatio{1.};
  double samplesToModelRatio{1.};
  int64_t samples_since_start{};
  double start_date{}; // in ns, for vst
  double cur_date{};

#if !defined(OSSIA_TESTING)
private:
#endif
  void init_midi_timings();
  void get_new_values();

  void register_parameter(ossia::net::parameter_base& p);
  void unregister_parameter(ossia::net::parameter_base& p);
  void register_midi_parameter(net::midi::midi_protocol& p);
  void unregister_midi_parameter(net::midi::midi_protocol& p);
  ossia::small_vector<ossia::net::device_base*, 4> m_devices_edit;
  ossia::small_vector<ossia::net::device_base*, 4> m_devices_exec;
  struct device_operation
  {
    enum
    {
      REGISTER,
      UNREGISTER
    } operation{};
    ossia::net::device_base* device{};
  };
  ossia::spsc_queue<device_operation> m_device_change_queue;

  std::list<message_queue> m_valueQueues;

  ossia::ptr_map<ossia::net::parameter_base*, value_vector<ossia::value>>
      m_receivedValues;

  struct received_midi_state
  {
    value_vector<libremidi::ump> messages;
    int64_t last_buffer_start{};
    int64_t current_buffer_start{};
    int count{};
  };

  ossia::ptr_map<ossia::net::midi::midi_protocol*, received_midi_state> m_receivedMidi;

  friend struct local_pull_visitor;
  friend struct global_pull_visitor;
  friend struct global_pull_node_visitor;
  friend struct state_exec_visitor;

  std::unique_ptr<execution_state_policy> m_policy;
};
}
