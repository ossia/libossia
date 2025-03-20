#pragma once
#include <ossia/audio/audio_parameter.hpp>
#include <ossia/dataflow/audio_lock.hpp>
#include <ossia/dataflow/data_copy.hpp>
#include <ossia/dataflow/execution/local_state_execution_policy.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/protocols/midi/detail/midi_impl.hpp>
#include <ossia/protocols/midi/midi_node.hpp>
namespace ossia
{

struct local_pull_visitor
{
  local_state_execution_policy& st;
  ossia::net::parameter_base* addr{};
  bool operator()(value_port& val) const
  {
    OSSIA_EXEC_STATE_LOCK_READ(st);
    auto it = st.m_valueState.find(addr);
    if(it != st.m_valueState.end() && !it->second.empty())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()(audio_port& val) const
  {
#if defined(OSSIA_PROTOCOL_AUDIO)
    OSSIA_EXEC_STATE_LOCK_READ(st);
    auto it = st.m_audioState.find(static_cast<ossia::audio_parameter*>(addr));
    if(it != st.m_audioState.end() && !it->second.empty())
    {
      copy_data{}(it->second, val);
      return true;
    }
#endif
    return false;
  }

  bool operator()(midi_port& val) const
  {
#if defined(OSSIA_PROTOCOL_MIDI)
    OSSIA_EXEC_STATE_LOCK_READ(st);
    auto it = st.m_midiState.find(addr);
    if(it != st.m_midiState.end() && !it->second.empty())
    {
      copy_data{}(it->second, val);
      return true;
    }
#endif
    return false;
  }

  [[noreturn]] bool operator()(geometry_port& val) const
  {
    assert(false);
    return false;
  }

  bool operator()() const { return false; }
};

struct global_pull_visitor
{
  ossia::execution_state& state;
  const net::parameter_base& out;
  void operator()(value_port& val) const
  {
    if(!val.is_event)
    {
      copy_data{}(out, val);
    }
    else
    {
      auto it = state.m_receivedValues.find(const_cast<net::parameter_base*>(&out));
      if(it != state.m_receivedValues.end())
      {
        copy_data{}(*it->first, it->second, val);
      }
    }
  }

  void operator()(audio_port& val) const
  {
#if defined(OSSIA_PROTOCOL_AUDIO)
#if !defined(NDEBUG)
    auto aa = dynamic_cast<const audio_parameter*>(&out);
    assert(aa);
#else
    auto aa = static_cast<const audio_parameter*>(&out);
#endif
    aa->clone_value(val.get());
#endif
  }

  void operator()(midi_port& val) const
  {
#if defined(OSSIA_PROTOCOL_MIDI)
    auto& node = out.get_node();
    auto& dev = node.get_device();
    auto& proto = dev.get_protocol();

#if !defined(NDEBUG)
    // TODO how to do without that dynamic_cast ?
    // Can we *ensure* that the address of the midi_port is a midi one ?
    auto midi = dynamic_cast<ossia::net::midi::midi_protocol*>(&proto);
    assert(midi);
#else
    auto midi = static_cast<ossia::net::midi::midi_protocol*>(&proto);
#endif

    auto it = state.m_receivedMidi.find(midi);
    if(it != state.m_receivedMidi.end())
    {
      for(const libremidi::ump& v : it->second.messages)
      {
        val.messages.push_back(v);
      }
    }
#endif
  }

  [[noreturn]] void operator()(geometry_port& val) const { assert(false); }

  void operator()() const { }
};

struct global_pull_node_visitor
{
  ossia::execution_state& state;
  const net::node_base& out;
  void operator()(value_port& val) const { val.write_value(ossia::net::to_map(out), 0); }

  void operator()(audio_port& val) const
  {
    // TODO Nothing to do ?
  }

  void operator()(midi_port& val) const
  {
#if defined(OSSIA_PROTOCOL_MIDI)
    auto& node = out;
    auto& dev = node.get_device();
    auto& proto = dev.get_protocol();

#if !defined(NDEBUG)
    // TODO how to do without that dynamic_cast ?
    // Can we *ensure* that the address of the midi_port is a midi one ?
    auto midi = dynamic_cast<ossia::net::midi::midi_protocol*>(&proto);
    assert(midi);
#else
    auto midi = static_cast<ossia::net::midi::midi_protocol*>(&proto);
#endif

    int channel = -1;
    if(node.get_parent() == &dev.get_root_node())
    {
      // the node is a MIDI channel node
      channel = static_cast<const ossia::net::midi::channel_node&>(node).channel;
    }

    auto it = state.m_receivedMidi.find(midi);
    if(it != state.m_receivedMidi.end())
    {
      if(channel == -1)
      {
        for(const libremidi::ump& v : it->second.messages)
        {
          val.messages.push_back(v);
        }
      }
      else
      {
        for(const libremidi::ump& v : it->second.messages)
        {
          if(v.get_channel() == channel)
            val.messages.push_back(v);
        }
      }
    }
#endif
  }

  [[noreturn]] void operator()(geometry_port& val) const { assert(false); }

  void operator()() const { }
};

}
