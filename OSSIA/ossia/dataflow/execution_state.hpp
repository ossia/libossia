#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_protocol.hpp>
#include <ossia/editor/state/flat_vec_state.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/mutex.hpp>
#include <smallfun.hpp>
namespace ossia
{
class message_queue;
struct local_pull_visitor;
struct global_pull_visitor;
struct state_exec_visitor;
#if defined(OSSIA_PARALLEL)
#define OSSIA_EXEC_STATE_LOCK_READ(state) ossia::read_lock_t ossia_read_lock{(state).mutex}
#define OSSIA_EXEC_STATE_LOCK_WRITE(state) ossia::write_lock_t ossia_write_lock{(state).mutex}
#else
#define OSSIA_EXEC_STATE_LOCK_READ(state)
#define OSSIA_EXEC_STATE_LOCK_WRITE(state)
#endif
struct OSSIA_EXPORT execution_state
    : public Nano::Observer
{
    execution_state();
    execution_state(const execution_state&) = delete;
    execution_state(execution_state&&) = delete;
    execution_state& operator=(const execution_state&) = delete;
    execution_state& operator=(execution_state&&) = delete;
    ~execution_state();

    /// To be called from the edition thread ///
    void register_device(ossia::net::device_base* d);
    void unregister_device(net::device_base* d);

    const ossia::small_vector<ossia::net::device_base*, 4>& edit_devices() const noexcept
    { return m_devices_edit; }


    /// To be called from the execution thread ///
    void register_inlet(const ossia::inlet& port);

    const ossia::small_vector<ossia::net::device_base*, 4>& exec_devices() const noexcept
    { return m_devices_exec; }
    ossia::net::node_base* find_node(std::string_view name) const noexcept
    {
      for(auto dev : m_devices_exec)
      {
        if(auto res = ossia::net::find_node(dev->get_root_node(), name))
          return res;
      }
      return nullptr;
    }

    template<typename T>
    auto get_value_or(std::string_view v, const T& val) noexcept
    {
      if(auto node = find_node(v))
        return ossia::convert<T>(node->get_parameter()->value());
      return val;
    }


    void begin_tick();

    void clear_devices();
    void reset();
    void commit();
    void commit_priorized();
    void commit_merged();
    void commit_ordered();
    void commit_common();

    void advance_tick(std::size_t);

    void find_and_copy(ossia::net::parameter_base& addr, inlet& in);
    void copy_from_global(ossia::net::parameter_base& addr, inlet& in);

    void insert(const ossia::destination& dest, const data_type& v);
    void insert(const ossia::destination& dest, data_type&& v);
    void insert(ossia::net::parameter_base& dest, const typed_value& v);
    void insert(ossia::net::parameter_base& dest, typed_value&& v);
    void insert(ossia::net::parameter_base& dest, const audio_port& v);
    void insert(ossia::net::parameter_base& dest, const midi_port& v);
    void insert(const ossia::state& v);

    bool in_local_scope(ossia::net::parameter_base& other) const;

    int sampleRate{44100};
    int bufferSize{64};
    int64_t samples_since_start{};
    double start_date{}; // in ns, for vst
    double cur_date{};

    // private:// disabled due to tests, but for some reason can't make friend work
    //using value_state_impl = ossia::flat_multimap<int64_t, std::pair<ossia::value, int>>;
    ossia::fast_hash_map<ossia::net::parameter_base*, value_vector<std::pair<typed_value, int>>> m_valueState;
    ossia::fast_hash_map<ossia::net::parameter_base*, audio_port> m_audioState;
    ossia::fast_hash_map<ossia::net::parameter_base*, value_vector<rtmidi::message>> m_midiState;

    mutable shared_mutex_t mutex;

  private:
    void get_new_values();
    void clear_local_state();

    void register_parameter(ossia::net::parameter_base& p);
    void unregister_parameter(ossia::net::parameter_base& p);
    void register_midi_parameter(net::midi::midi_protocol& p);
    ossia::small_vector<ossia::net::device_base*, 4> m_devices_edit;
    ossia::small_vector<ossia::net::device_base*, 4> m_devices_exec;
    struct device_operation
    {
      enum { REGISTER, UNREGISTER } operation{};
      ossia::net::device_base* device{};
    };
    moodycamel::ReaderWriterQueue<device_operation> m_device_change_queue;

    std::list<message_queue> m_valueQueues;

    ossia::ptr_map<ossia::net::parameter_base*, value_vector<ossia::value>> m_receivedValues;
    ossia::ptr_map<ossia::net::midi::midi_protocol*, value_vector<rtmidi::message>> m_receivedMidi;

    ossia::mono_state m_monoState;
    ossia::flat_vec_state m_commitOrderedState;
    ossia::flat_map<std::pair<int64_t, int>, std::vector<ossia::state_element>> m_flatMessagesCache;

    int m_msgIndex{};

    friend struct local_pull_visitor;
    friend struct global_pull_visitor;
    friend struct state_exec_visitor;
};
}
