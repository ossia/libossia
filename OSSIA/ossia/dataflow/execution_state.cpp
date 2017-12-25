// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/audio_parameter.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/detail/apply.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/network/midi/midi_device.hpp>
#include <ossia/network/midi/midi_parameter.hpp>
namespace ossia
{

struct local_pull_visitor
{
  execution_state& st;
  destination_t addr;
  bool operator()(value_port& val)
  {
    auto it = st.m_valueState.find(addr);
    if (it != st.m_valueState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()(audio_port& val)
  {
    auto it = st.m_audioState.find(addr);
    if (it != st.m_audioState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()(midi_port& val)
  {
    auto it = st.m_midiState.find(addr);
    if (it != st.m_midiState.end())
    {
      copy_data{}(it->second, val);
      return true;
    }
    return false;
  }

  bool operator()()
  {
    return false;
  }
};

// TODO if addresses are missing should they be created automatically ?
// Else how to create a custom address
struct global_pull_visitor
{
  ossia::execution_state& state;
  const net::parameter_base& out;
  void operator()(value_port& val)
  {
    if(!val.is_event)
    {
      val.add_raw_value(out.value());
    }
    else
    {
      auto it = state.m_receivedValues.find(const_cast<net::parameter_base*>(&out));
      if(it != state.m_receivedValues.end())
      {
        for(auto& v : it->second)
          val.add_raw_value(std::move(v));
      }
    }
  }

  void operator()(audio_port& val)
  {
    auto aa = dynamic_cast<const audio_parameter*>(&out);
    assert(aa);
    aa->clone_value(val.samples);
  }

  void operator()(midi_port& val)
  {
    auto& proto = out.get_node().get_device().get_protocol();
    if(auto midi = dynamic_cast<ossia::net::midi::midi_protocol*>(&proto))
    {
      auto it = state.m_receivedMidi.find(midi);
      if(it != state.m_receivedMidi.end())
      {
        for(auto& v : it->second)
        {
          val.messages.push_back(std::move(v));
        }
      }
    }
  }

  void operator()()
  {
  }
};

void execution_state::register_device(net::device_base* d)
{
  valueDevices.push_back(d);
  m_valueQueues.emplace_back(*d);
}

void execution_state::register_device(net::midi::midi_device* d)
{
  valueDevices.push_back(d);
  midiDevices.push_back(static_cast<ossia::net::midi::midi_protocol*>(&d->get_protocol()));
  m_valueQueues.emplace_back(*d);
}

void execution_state::register_parameter(net::parameter_base& p)
{
  for(std::size_t i = 0; i < valueDevices.size(); i++)
  {
    if(&p.get_node().get_device() == valueDevices[i])
    {
      auto it = m_valueQueues.begin();
      std::advance(it, i);
      it->reg(p);
      return;
    }
  }
}

void execution_state::unregister_parameter(net::parameter_base& p)
{
  for(std::size_t i = 0; i < valueDevices.size(); i++)
  {
    if(&p.get_node().get_device() == valueDevices[i])
    {
      auto it = m_valueQueues.begin();
      std::advance(it, i);
      it->unreg(p);
      return;
    }
  }
}

void execution_state::register_midi_parameter(net::midi::midi_protocol& p)
{
  p.enable_registration();
  m_receivedMidi.insert({&p, {}});
}

void execution_state::get_new_values()
{
  for(auto it = m_receivedValues.begin(), end = m_receivedValues.end(); it != end; ++it)
    it.value().clear();

  for(auto& mq : m_valueQueues)
  {
    ossia::received_value recv;
    while(mq.try_dequeue(recv))
      m_receivedValues[recv.address].push_back(recv.value);
  }

  for(auto it = m_receivedMidi.begin(), end = m_receivedMidi.end(); it != end; ++it)
  {
    it.value().clear();
    it->first->clone_value(it.value());
  }
}

void execution_state::register_inlet(const inlet& port)
{
  if(auto vp = port.data.target<ossia::value_port>())
  {
    if(vp->is_event)
    {
      if(auto addr = port.address.target<ossia::net::parameter_base*>())
        register_parameter(**addr);
    }
  }
  else if(auto mp = port.data.target<ossia::midi_port>())
  {
    if(auto addr = port.address.target<ossia::net::node_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(&(*addr)->get_device().get_protocol()))
      {
        register_midi_parameter(*midi_addr);
      }
    }
    else if(auto addr = port.address.target<ossia::net::parameter_base*>())
    {
      if(auto midi_addr = dynamic_cast<ossia::net::midi::midi_protocol*>(&(*addr)->get_node().get_device().get_protocol()))
      {
        register_midi_parameter(*midi_addr);
      }
    }
  }
}

void execution_state::clear()
{
  m_valueState.clear();
  m_audioState.clear();
  m_midiState.clear();

}
void execution_state::reset()
{
  // TODO unregister everything ?
  clear();
  valueDevices.clear();
  midiDevices.clear();
  m_valueQueues.clear();
  m_receivedValues.clear();
  m_receivedMidi.clear();
}

ossia::state_element to_state_element(ossia::net::parameter_base& p, ossia::tvalue&& v)
{
  ossia::message m{p, std::move(v.value)};
  if(auto u = v.type.target<ossia::unit_t>())
    m.dest.unit = std::move(*u);
  m.dest.index = std::move(v.index);
  return m;
}
void execution_state::commit()
{
  ossia::state st;
  st.reserve(m_valueState.size());
  for (auto it = m_valueState.begin(), end = m_valueState.end(); it != end; ++it)
  {
    apply_to_destination(it->first, *this, [&] (ossia::net::parameter_base* addr) {
      for(auto& val : it.value())
        ossia::flatten_and_filter(st, to_state_element(*addr, std::move(val)));
    });
  }
  st.launch();

  for (auto& elt : m_audioState)
  {
    apply_to_destination(elt.first, *this, [&] (ossia::net::parameter_base* base_addr) {
      auto addr = dynamic_cast<audio_parameter*>(base_addr);
      if(addr)
        addr->push_value(elt.second);
    });
  }

  for (auto& elt : m_midiState)
  {
    apply_to_destination(elt.first, *this, [&] (ossia::net::parameter_base* base_addr) {
      if(auto addr = dynamic_cast<ossia::net::midi::midi_parameter*>(base_addr))
      {
        auto& proto = static_cast<ossia::net::midi::midi_protocol&>(addr->get_protocol());

        for (const auto& v : elt.second)
        {
          proto.push_value(v);
        }
      }
    });
  }
}

void execution_state::find_and_copy(net::parameter_base& addr, inlet& in)
{
  if (!ossia::apply(local_pull_visitor{*this, &addr}, in.data))
  {
    copy_from_global(addr, in);
  }
}

void execution_state::copy_from_global(net::parameter_base& addr, inlet& in)
{
  if (in.scope & port::scope_t::global)
  {
    ossia::apply(global_pull_visitor{*this, addr}, in.data);
  }
}

void execution_state::insert(const destination_t& dest, data_type v)
{
  using namespace eggs::variants;
  if (auto audio = eggs::variants::get_if<audio_port>(&v))
  {
    insert(dest, std::move(*audio));
  }
  else if (auto midi = eggs::variants::get_if<midi_port>(&v))
  {
    insert(dest, std::move(*midi));
  }
  else if (auto val = eggs::variants::get_if<value_port>(&v))
  {
    for(auto& v : val->get_data())
      insert(dest, v);
  }
}

bool execution_state::in_local_scope(net::parameter_base& other) const
{
  destination_t dest{&other};
  bool ok = (m_valueState.find(dest) != m_valueState.end())
            || (m_audioState.find(dest) != m_audioState.end())
            || (m_midiState.find(dest) != m_midiState.end());
  if (!ok)
  {
    // TODO check if there is any pattern matching the current destination
  }
  return ok;
}
}
