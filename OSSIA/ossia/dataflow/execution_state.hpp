#pragma once
#include <ossia/dataflow/data.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/message_queue.hpp>
#include <unordered_map>
namespace ossia
{
struct OSSIA_EXPORT execution_state
    : public Nano::Observer
{
    int sampleRate{44100};
    int bufferSize{64};

    ossia::net::node_base* find_node(std::string_view name)
    {
      for(auto dev : globalState)
      {
        if(auto res = ossia::net::find_node(dev->get_root_node(), name))
          return res;
      }
      return nullptr;
    }

    void clear_devices()
    {
      globalState.clear();
      messages.clear();
    }

    void register_device(ossia::net::device_base* d)
    {
      globalState.push_back(d);
      messages.emplace_back(*d);
    }

    void register_parameter(ossia::net::parameter_base& p)
    {
      for(std::size_t i = 0; i < globalState.size(); i++)
      {
        if(&p.get_node().get_device() == globalState[i])
        {
          auto it = messages.begin();
          std::advance(it, i);
          it->reg(p);
        }
      }
    }
    void unregister_parameter(ossia::net::parameter_base& p)
    {
      for(std::size_t i = 0; i < globalState.size(); i++)
      {
        if(&p.get_node().get_device() == globalState[i])
        {
          auto it = messages.begin();
          std::advance(it, i);
          it->unreg(p);
        }
      }
    }

    void get_new_values()
    {
      mess_values.clear();
      for(auto& mq : messages)
      {
        ossia::received_value recv;
        while(mq.try_dequeue(recv))
          mess_values[recv.address].push_back(recv.value);
      }
    }

  std::list<message_queue> messages;
  std::unordered_map<ossia::net::parameter_base*, value_vector<ossia::value>> mess_values;

  std::vector<ossia::net::device_base*> globalState;

  std::unordered_map<destination_t, std::vector<tvalue>> valueState;
  std::unordered_map<destination_t, audio_port> audioState;

#if defined(OSSIA_PROTOCOL_MIDI)
  std::unordered_map<destination_t, value_vector<mm::MidiMessage>> midiState;
#endif

  void clear();

  void commit();

  void find_and_copy(ossia::net::parameter_base& addr, inlet& in);

  void copy_from_global(ossia::net::parameter_base& addr, inlet& in);

  // todo separate rvalue & cref
  void insert(const destination_t& dest, data_type v);
  void insert(const destination_t& dest, tvalue v)
  {
    valueState[dest].push_back(std::move(v));
  }
  void insert(const destination_t& dest, const audio_port& v)
  {
    audioState[dest] = v;
  }
#if defined(OSSIA_PROTOCOL_MIDI)
  void insert(const destination_t& dest, const midi_port& v)
  {
    midiState[dest] = v.messages;
  }
#endif
  bool in_local_scope(ossia::net::parameter_base& other) const;
};
}
