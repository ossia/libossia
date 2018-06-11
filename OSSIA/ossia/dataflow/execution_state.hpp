#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_protocol.hpp>
#include <ossia/editor/state/flat_vec_state.hpp>
#include <ossia/editor/state/state.hpp>
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/flat_multimap.hpp>
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/mutex.hpp>
namespace ossia
{
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
    ossia::net::node_base* find_node(std::string_view name) const
    {
      for(auto dev : allDevices)
      {
        if(auto res = ossia::net::find_node(dev->get_root_node(), name))
          return res;
      }
      return nullptr;
    }

    template<typename T>
    auto get_value_or(std::string_view v, const T& val)
    {
      if(auto node = find_node(v))
        return ossia::convert<T>(node->get_parameter()->value());
      return val;
    }

    void register_device(ossia::net::device_base* d);
    void register_device(ossia::net::midi::midi_device* d);

    void register_parameter(ossia::net::parameter_base& p);
    void unregister_parameter(ossia::net::parameter_base& p);
    void register_midi_parameter(net::midi::midi_protocol& p);
    void get_new_values();

    void register_inlet(const ossia::inlet& port);


    void clear_local_state();

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

    // todo separate rvalue & cref
    void insert(ossia::net::parameter_base& dest, const data_type& v);
    void insert(ossia::net::parameter_base& dest, data_type&& v);
    void insert(ossia::net::parameter_base& dest, const tvalue& v);
    void insert(ossia::net::parameter_base& dest, tvalue&& v);
    void insert(ossia::net::parameter_base& dest, const audio_port& v);
    void insert(ossia::net::parameter_base& dest, const midi_port& v);
    void insert(const ossia::state& v);

    bool in_local_scope(ossia::net::parameter_base& other) const;

    int sampleRate{44100};
    int bufferSize{64};
    int64_t samples_since_start{};
    double start_date{}; // in ns, for vst
    double cur_date{};

    ossia::small_vector<ossia::net::device_base*, 4> valueDevices;
    ossia::small_vector<ossia::net::midi::midi_protocol*, 2> midiDevices;
    ossia::small_vector<ossia::net::device_base*, 2> audioDevices;

    ossia::small_vector<ossia::net::device_base*, 4> allDevices;

    // private:// disabled due to tests, but for some reason can't make friend work
    using value_state_impl = ossia::flat_multimap<int64_t, std::pair<ossia::value, int>>;
    ossia::fast_hash_map<ossia::net::parameter_base*, value_vector<std::pair<tvalue, int>>> m_valueState;
    ossia::fast_hash_map<ossia::net::parameter_base*, audio_port> m_audioState;
    ossia::fast_hash_map<ossia::net::parameter_base*, value_vector<rtmidi::message>> m_midiState;

    mutable shared_mutex_t mutex;

  private:
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
