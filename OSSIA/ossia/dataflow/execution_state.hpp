#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/editor/state/flat_state.hpp>
#include <ossia/editor/state/state.hpp>
#include <boost/container/flat_map.hpp>
#include <ossia/detail/hash_map.hpp>
namespace ossia
{
struct OSSIA_EXPORT execution_state
    : public Nano::Observer
{
    execution_state();
    ossia::net::node_base* find_node(std::string_view name)
    {
      for(auto dev : valueDevices)
      {
        if(auto res = ossia::net::find_node(dev->get_root_node(), name))
          return res;
      }
      return nullptr;
    }

    template<typename T>
    auto get_value_or(std::string_view v, T val)
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

    void find_and_copy(ossia::net::parameter_base& addr, inlet& in);
    void copy_from_global(ossia::net::parameter_base& addr, inlet& in);

    // todo separate rvalue & cref
    void insert(ossia::net::parameter_base& dest, const data_type& v);
    void insert(ossia::net::parameter_base& dest, data_type&& v);
    void insert(ossia::net::parameter_base& dest, const tvalue& v);
    void insert(ossia::net::parameter_base& dest, tvalue&& v);
    void insert(ossia::net::parameter_base& dest, const audio_port& v);
    void insert(ossia::net::parameter_base& dest, const midi_port& v);

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
    using value_state_impl = boost::container::flat_multimap<int64_t, std::pair<ossia::value, int>>;
    ossia::fast_hash_map<ossia::net::parameter_base*, value_vector<std::pair<tvalue, int>>> m_valueState;
    ossia::fast_hash_map<ossia::net::parameter_base*, audio_port> m_audioState;
    ossia::fast_hash_map<ossia::net::parameter_base*, value_vector<mm::MidiMessage>> m_midiState;

    std::list<message_queue> m_valueQueues;

    ossia::ptr_map<ossia::net::parameter_base*, value_vector<ossia::value>> m_receivedValues;
    ossia::ptr_map<ossia::net::midi::midi_protocol*, value_vector<mm::MidiMessage>> m_receivedMidi;

    ossia::mono_state m_monoState;
    ossia::flat_vec_state m_commitOrderedState;
    boost::container::flat_map<std::pair<int64_t, int>, std::vector<ossia::state_element>> m_flatMessagesCache;

    int m_msgIndex{};

};
}
