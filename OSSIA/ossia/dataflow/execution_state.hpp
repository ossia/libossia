#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <ossia/detail/ptr_set.hpp>
#include <ossia/network/midi/midi_device.hpp>

namespace ossia
{
struct OSSIA_EXPORT execution_state
    : public Nano::Observer
{
    ossia::net::node_base* find_node(std::string_view name)
    {
      for(auto dev : valueDevices)
      {
        if(auto res = ossia::net::find_node(dev->get_root_node(), name))
          return res;
      }
      return nullptr;
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
    void find_and_copy(ossia::net::parameter_base& addr, inlet& in);
    void copy_from_global(ossia::net::parameter_base& addr, inlet& in);

    // todo separate rvalue & cref
    void insert(const destination_t& dest, data_type v);
    void insert(const destination_t& dest, tvalue v);
    void insert(const destination_t& dest, const audio_port& v);
    void insert(const destination_t& dest, const midi_port& v);

    bool in_local_scope(ossia::net::parameter_base& other) const;

    int sampleRate{44100};
    int bufferSize{64};
    int64_t samples_since_start{};
    std::chrono::high_resolution_clock::time_point start_date;
    std::chrono::high_resolution_clock::time_point cur_date;

    ossia::small_vector<ossia::net::device_base*, 4> valueDevices;
    ossia::small_vector<ossia::net::midi::midi_protocol*, 2> midiDevices;
    ossia::small_vector<ossia::net::device_base*, 2> audioDevices;

    ossia::small_vector<ossia::net::device_base*, 4> allDevices;

    // private:// disabled due to tests, but for some reason can't make friend work
    tsl::hopscotch_map<destination_t, value_vector<tvalue>> m_valueState;
    tsl::hopscotch_map<destination_t, audio_port> m_audioState;
    tsl::hopscotch_map<destination_t, value_vector<mm::MidiMessage>> m_midiState;

    std::list<message_queue> m_valueQueues;

    ossia::ptr_map<ossia::net::parameter_base*, value_vector<ossia::value>> m_receivedValues;
    ossia::ptr_map<ossia::net::midi::midi_protocol*, value_vector<mm::MidiMessage>> m_receivedMidi;
};
}
